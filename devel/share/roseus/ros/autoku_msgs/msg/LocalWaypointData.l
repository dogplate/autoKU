;; Auto-generated. Do not edit!


(when (boundp 'autoku_msgs::LocalWaypointData)
  (if (not (find-package "AUTOKU_MSGS"))
    (make-package "AUTOKU_MSGS"))
  (shadow 'LocalWaypointData (find-package "AUTOKU_MSGS")))
(unless (find-package "AUTOKU_MSGS::LOCALWAYPOINTDATA")
  (make-package "AUTOKU_MSGS::LOCALWAYPOINTDATA"))

(in-package "ROS")
;;//! \htmlinclude LocalWaypointData.msg.html
(if (not (find-package "GEOMETRY_MSGS"))
  (ros::roseus-add-msgs "geometry_msgs"))


(defclass autoku_msgs::LocalWaypointData
  :super ros::object
  :slots (_frame_id _points _target_velocity ))

(defmethod autoku_msgs::LocalWaypointData
  (:init
   (&key
    ((:frame_id __frame_id) "")
    ((:points __points) (let (r) (dotimes (i 0) (push (instance geometry_msgs::Point :init) r)) r))
    ((:target_velocity __target_velocity) (make-array 0 :initial-element 0.0 :element-type :float))
    )
   (send-super :init)
   (setq _frame_id (string __frame_id))
   (setq _points __points)
   (setq _target_velocity __target_velocity)
   self)
  (:frame_id
   (&optional __frame_id)
   (if __frame_id (setq _frame_id __frame_id)) _frame_id)
  (:points
   (&rest __points)
   (if (keywordp (car __points))
       (send* _points __points)
     (progn
       (if __points (setq _points (car __points)))
       _points)))
  (:target_velocity
   (&optional __target_velocity)
   (if __target_velocity (setq _target_velocity __target_velocity)) _target_velocity)
  (:serialization-length
   ()
   (+
    ;; string _frame_id
    4 (length _frame_id)
    ;; geometry_msgs/Point[] _points
    (apply #'+ (send-all _points :serialization-length)) 4
    ;; float64[] _target_velocity
    (* 8    (length _target_velocity)) 4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; string _frame_id
       (write-long (length _frame_id) s) (princ _frame_id s)
     ;; geometry_msgs/Point[] _points
     (write-long (length _points) s)
     (dolist (elem _points)
       (send elem :serialize s)
       )
     ;; float64[] _target_velocity
     (write-long (length _target_velocity) s)
     (dotimes (i (length _target_velocity))
       (sys::poke (elt _target_velocity i) (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
       )
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; string _frame_id
     (let (n) (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4) (setq _frame_id (subseq buf ptr- (+ ptr- n))) (incf ptr- n))
   ;; geometry_msgs/Point[] _points
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _points (let (r) (dotimes (i n) (push (instance geometry_msgs::Point :init) r)) r))
     (dolist (elem- _points)
     (send elem- :deserialize buf ptr-) (incf ptr- (send elem- :serialization-length))
     ))
   ;; float64[] _target_velocity
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _target_velocity (instantiate float-vector n))
     (dotimes (i n)
     (setf (elt _target_velocity i) (sys::peek buf ptr- :double)) (incf ptr- 8)
     ))
   ;;
   self)
  )

(setf (get autoku_msgs::LocalWaypointData :md5sum-) "f152e1e888768e9d62f27b1539b9e86d")
(setf (get autoku_msgs::LocalWaypointData :datatype-) "autoku_msgs/LocalWaypointData")
(setf (get autoku_msgs::LocalWaypointData :definition-)
      "string frame_id
geometry_msgs/Point[] points
float64[] target_velocity
================================================================================
MSG: geometry_msgs/Point
# This contains the position of a point in free space
float64 x
float64 y
float64 z

")



(provide :autoku_msgs/LocalWaypointData "f152e1e888768e9d62f27b1539b9e86d")


