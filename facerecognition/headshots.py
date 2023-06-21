import os, cv2, sys, getopt

def main(argv):
    name = '' # name to cpature (must create dataset dir first)
    try:
        opts, args = getopt.getopt(argv,"n:",["name="])
    except:
        exit(2)
    for opt, arg in opts:
        if opt in ("-n", "--name"):
            name = arg
        else:
            exit()
    print ('Capturing images of', name)

    parent_dir = "dataset"
    if not os.path.exists(parent_dir):
        os.makedirs(parent_dir)
    directory = name
    path = os.path.join(parent_dir, directory)
    if not os.path.exists(path):
        os.mkdir(path)
    else:
        print("Dataset exits - warning - will overwrite images")

    cam = cv2.VideoCapture(0)

    # cv2.namedWindow("press space to take a photo", cv2.WINDOW_NORMAL)
    cv2.namedWindow("press space to take a photo", 1000)
    # cv2.resizeWindow("press space to take a photo", 500, 300)

    img_counter = 0

    while True:
        ret, frame = cam.read()
        if not ret:
            print("failed to grab frame")
            break
        cv2.imshow("press space to take a photo", frame)

        k = cv2.waitKey(1)
        if k%256 == 27:
            # ESC pressed
            print("Escape hit, closing...")
            break
        elif k%256 == 32:
            # SPACE pressed
            img_name = "dataset/"+ name +"/image_{}.jpg".format(img_counter)
            cv2.imwrite(img_name, frame)
            print("{} written!".format(img_name))
            img_counter += 1

    cam.release()

    cv2.destroyAllWindows()

if __name__ == "__main__":
   main(sys.argv[1:])
