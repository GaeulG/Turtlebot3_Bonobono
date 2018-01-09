; Auto-generated. Do not edit!


(cl:in-package oroca_ros_tutorials-msg)


;//! \htmlinclude msgTutorial.msg.html

(cl:defclass <msgTutorial> (roslisp-msg-protocol:ros-message)
  ((data
    :reader data
    :initarg :data
    :type cl:integer
    :initform 0))
)

(cl:defclass msgTutorial (<msgTutorial>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <msgTutorial>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'msgTutorial)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name oroca_ros_tutorials-msg:<msgTutorial> is deprecated: use oroca_ros_tutorials-msg:msgTutorial instead.")))

(cl:ensure-generic-function 'data-val :lambda-list '(m))
(cl:defmethod data-val ((m <msgTutorial>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader oroca_ros_tutorials-msg:data-val is deprecated.  Use oroca_ros_tutorials-msg:data instead.")
  (data m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <msgTutorial>) ostream)
  "Serializes a message object of type '<msgTutorial>"
  (cl:let* ((signed (cl:slot-value msg 'data)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <msgTutorial>) istream)
  "Deserializes a message object of type '<msgTutorial>"
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'data) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<msgTutorial>)))
  "Returns string type for a message object of type '<msgTutorial>"
  "oroca_ros_tutorials/msgTutorial")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'msgTutorial)))
  "Returns string type for a message object of type 'msgTutorial"
  "oroca_ros_tutorials/msgTutorial")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<msgTutorial>)))
  "Returns md5sum for a message object of type '<msgTutorial>"
  "da5909fbe378aeaf85e547e830cc1bb7")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'msgTutorial)))
  "Returns md5sum for a message object of type 'msgTutorial"
  "da5909fbe378aeaf85e547e830cc1bb7")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<msgTutorial>)))
  "Returns full string definition for message of type '<msgTutorial>"
  (cl:format cl:nil "int32 data~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'msgTutorial)))
  "Returns full string definition for message of type 'msgTutorial"
  (cl:format cl:nil "int32 data~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <msgTutorial>))
  (cl:+ 0
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <msgTutorial>))
  "Converts a ROS message object to a list"
  (cl:list 'msgTutorial
    (cl:cons ':data (data msg))
))
