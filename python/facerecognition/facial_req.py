#! /usr/bin/python
from imutils.video import VideoStream
from imutils.video import FPS
import face_recognition
import struct
import socket
import imutils
import pickle
import time
import cv2
import sys

SendFrames = False
clientsocket=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
try:
    clientsocket.connect(('localhost',8089))
except:
    SendFrames = False

#Initialize 'currentname' to trigger only when a new person is identified.
currentname = "unknown"
#Determine faces from encodings.pickle file model created from train_model.py
encodingsP = "encodings.pickle"

# load the known faces and embeddings along with OpenCV's Haar
# cascade for face detection
data = pickle.loads(open(encodingsP, "rb").read())
# print(data);

# initialize the video stream and allow the camera sensor to warm up
# Set the ser to the followng
# src = 0 : for the build in single web cam, could be your laptop webcam
# src = 2 : I had to set it to 2 inorder to use the USB webcam attached to my laptop
vs = VideoStream(src=0,framerate=32).start()

# start the FPS counter
fps = FPS().start()

Display = True

# loop over frames from the video file stream
while 1:
    shutdown = open("/tmp/shutdown.hamza", "r")
    if "1" in shutdown.read():
        break
    # time.sleep(0.1)

    # grab the frame from the threaded video stream and resize it
    # to 500px (to speedup processing)
    frame = vs.read()
    frame = imutils.resize(frame, width=500)

    # Detect the fce boxes
    boxes = face_recognition.face_locations(frame)
    # compute the facial embeddings for each face bounding box
    encodings = face_recognition.face_encodings(frame, boxes)
    names = []
   # loop over the facial embeddings
    for encoding in encodings:
        # attempt to match each face in the input image to our known
        # encodings
        matches = face_recognition.compare_faces(data["encodings"],
            encoding)
        name = "Unknown" #if face is not recognized, then print Unknown

        # check to see if we have found a match
        if True in matches:
            # find the indexes of all matched faces then initialize a
            # dictionary to count the total number of times each face
            # was matched
            matchedIdxs = [i for (i, b) in enumerate(matches) if b]
            counts = {}

            # loop over the matched indexes and maintain a count for
            # each recognized face face
            for i in matchedIdxs:
                name = data["names"][i]
                counts[name] = counts.get(name, 0) + 1

            # determine the recognized face with the largest number
            # of votes (note: in the event of an unlikely tie Python
            # will select first entry in the dictionary)
            name = max(counts, key=counts.get)
            sys.stdout.write(str(name) + "\n")
            sys.stdout.flush()

            #If someone in your dataset is identified, print their name on the screen
            if currentname != name:
                currentname = name

        # update the list of names
        names.append(name)

    # loop over the recognized faces
    for ((top, right, bottom, left), name) in zip(boxes, names):
        # draw the predicted face name on the image - color is in BGR
        cv2.rectangle(frame, (left, top), (right, bottom),
            (52, 165, 111), 2)
        y = top - 15 if top - 15 > 15 else top + 15
        cv2.putText(frame, name, (left, y), cv2.FONT_HERSHEY_SIMPLEX,
            .8, (52, 165, 111), 2)

    if Display:
        # display the image to our screen
        # key: (check cv::WindowFlags)
        # floating = 1
        # docked with buttons = 10
        # floating no border = 17
        # docked no buttons = 18
        # stretched docked = 1000
        cv2.namedWindow("FR", 1000)
        cv2.imshow("FR", frame)
        key = cv2.waitKey(1) & 0xFF

        # quit when 'q' key is pressed
        if key == ord("q"):
            break

    # update the FPS counter
    fps.update()

    if not SendFrames:
        try:
            clientsocket.connect(('localhost',8089))
        except:
            SendFrames = False
    else:
        try:
            # Serialize frame
            serialframe = pickle.dumps(frame)
            # Send message length first
            message_size = struct.pack("L", len(serialframe))
            # Then data
            clientsocket.sendall(message_size + serialframe)
        except:
            SendFrames = False

# stop the timer and display FPS information
fps.stop()
print("[INFO] elasped time: {:.2f}".format(fps.elapsed()))
print("[INFO] approx. FPS: {:.2f}".format(fps.fps()))

# do a bit of cleanup
if Display:
    cv2.destroyAllWindows()
vs.stop()
