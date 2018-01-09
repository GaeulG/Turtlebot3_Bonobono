;; Auto-generated. Do not edit!


(when (boundp 'turtlebot_pkg::msgTutorial)
  (if (not (find-package "TURTLEBOT_PKG"))
    (make-package "TURTLEBOT_PKG"))
  (shadow 'msgTutorial (find-package "TURTLEBOT_PKG")))
(unless (find-package "TURTLEBOT_PKG::MSGTUTORIAL")
  (make-package "TURTLEBOT_PKG::MSGTUTORIAL"))

(in-package "ROS")
;;//! \htmlinclude msgTutorial.msg.html


(defclass turtlebot_pkg::msgTutorial
  :super ros::object
  :slots (_data ))

(defmethod turtlebot_pkg::msgTutorial
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

(setf (get turtlebot_pkg::msgTutorial :md5sum-) "da5909fbe378aeaf85e547e830cc1bb7")
(setf (get turtlebot_pkg::msgTutorial :datatype-) "turtlebot_pkg/msgTutorial")
(setf (get turtlebot_pkg::msgTutorial :definition-)
      "int32 data

")



(provide :turtlebot_pkg/msgTutorial "da5909fbe378aeaf85e547e830cc1bb7")


