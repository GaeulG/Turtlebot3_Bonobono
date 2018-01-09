;; Auto-generated. Do not edit!


(when (boundp 'opencv_tutorial::msgTutorial)
  (if (not (find-package "OPENCV_TUTORIAL"))
    (make-package "OPENCV_TUTORIAL"))
  (shadow 'msgTutorial (find-package "OPENCV_TUTORIAL")))
(unless (find-package "OPENCV_TUTORIAL::MSGTUTORIAL")
  (make-package "OPENCV_TUTORIAL::MSGTUTORIAL"))

(in-package "ROS")
;;//! \htmlinclude msgTutorial.msg.html


(defclass opencv_tutorial::msgTutorial
  :super ros::object
  :slots (_data ))

(defmethod opencv_tutorial::msgTutorial
  (:init
   (&key
    ((:data __data) 0)
    )
   (send-super :init)
   (setq _data (round __data))
   self)
  (:data
   (&optional __data)
   (if __data (setq _data __data)) _data)
  (:serialization-length
   ()
   (+
    ;; int32 _data
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; int32 _data
       (write-long _data s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; int32 _data
     (setq _data (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;;
   self)
  )

(setf (get opencv_tutorial::msgTutorial :md5sum-) "da5909fbe378aeaf85e547e830cc1bb7")
(setf (get opencv_tutorial::msgTutorial :datatype-) "opencv_tutorial/msgTutorial")
(setf (get opencv_tutorial::msgTutorial :definition-)
      "int32 data

")



(provide :opencv_tutorial/msgTutorial "da5909fbe378aeaf85e547e830cc1bb7")


