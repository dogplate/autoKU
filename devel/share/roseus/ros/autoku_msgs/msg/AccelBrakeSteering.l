;; Auto-generated. Do not edit!


(when (boundp 'autoku_msgs::AccelBrakeSteering)
  (if (not (find-package "AUTOKU_MSGS"))
    (make-package "AUTOKU_MSGS"))
  (shadow 'AccelBrakeSteering (find-package "AUTOKU_MSGS")))
(unless (find-package "AUTOKU_MSGS::ACCELBRAKESTEERING")
  (make-package "AUTOKU_MSGS::ACCELBRAKESTEERING"))

(in-package "ROS")
;;//! \htmlinclude AccelBrakeSteering.msg.html


(defclass autoku_msgs::AccelBrakeSteering
  :super ros::object
  :slots (_accel _brake _frontAngle ))

(defmethod autoku_msgs::AccelBrakeSteering
  (:init
   (&key
    ((:accel __accel) 0.0)
    ((:brake __brake) 0.0)
    ((:frontAngle __frontAngle) 0.0)
    )
   (send-super :init)
   (setq _accel (float __accel))
   (setq _brake (float __brake))
   (setq _frontAngle (float __frontAngle))
   self)
  (:accel
   (&optional __accel)
   (if __accel (setq _accel __accel)) _accel)
  (:brake
   (&optional __brake)
   (if __brake (setq _brake __brake)) _brake)
  (:frontAngle
   (&optional __frontAngle)
   (if __frontAngle (setq _frontAngle __frontAngle)) _frontAngle)
  (:serialization-length
   ()
   (+
    ;; float64 _accel
    8
    ;; float64 _brake
    8
    ;; float64 _frontAngle
    8
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; float64 _accel
       (sys::poke _accel (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
     ;; float64 _brake
       (sys::poke _brake (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
     ;; float64 _frontAngle
       (sys::poke _frontAngle (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; float64 _accel
     (setq _accel (sys::peek buf ptr- :double)) (incf ptr- 8)
   ;; float64 _brake
     (setq _brake (sys::peek buf ptr- :double)) (incf ptr- 8)
   ;; float64 _frontAngle
     (setq _frontAngle (sys::peek buf ptr- :double)) (incf ptr- 8)
   ;;
   self)
  )

(setf (get autoku_msgs::AccelBrakeSteering :md5sum-) "27a9643207b32f2deac0ed64a59c362d")
(setf (get autoku_msgs::AccelBrakeSteering :datatype-) "autoku_msgs/AccelBrakeSteering")
(setf (get autoku_msgs::AccelBrakeSteering :definition-)
      "float64 accel
float64 brake
float64 frontAngle

")



(provide :autoku_msgs/AccelBrakeSteering "27a9643207b32f2deac0ed64a59c362d")


