;; Auto-generated. Do not edit!


(when (boundp 'autoku_msgs::VehicleInput)
  (if (not (find-package "AUTOKU_MSGS"))
    (make-package "AUTOKU_MSGS"))
  (shadow 'VehicleInput (find-package "AUTOKU_MSGS")))
(unless (find-package "AUTOKU_MSGS::VEHICLEINPUT")
  (make-package "AUTOKU_MSGS::VEHICLEINPUT"))

(in-package "ROS")
;;//! \htmlinclude VehicleInput.msg.html


(defclass autoku_msgs::VehicleInput
  :super ros::object
  :slots (_steering _accel _brake ))

(defmethod autoku_msgs::VehicleInput
  (:init
   (&key
    ((:steering __steering) 0.0)
    ((:accel __accel) 0.0)
    ((:brake __brake) 0.0)
    )
   (send-super :init)
   (setq _steering (float __steering))
   (setq _accel (float __accel))
   (setq _brake (float __brake))
   self)
  (:steering
   (&optional __steering)
   (if __steering (setq _steering __steering)) _steering)
  (:accel
   (&optional __accel)
   (if __accel (setq _accel __accel)) _accel)
  (:brake
   (&optional __brake)
   (if __brake (setq _brake __brake)) _brake)
  (:serialization-length
   ()
   (+
    ;; float64 _steering
    8
    ;; float64 _accel
    8
    ;; float64 _brake
    8
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; float64 _steering
       (sys::poke _steering (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
     ;; float64 _accel
       (sys::poke _accel (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
     ;; float64 _brake
       (sys::poke _brake (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; float64 _steering
     (setq _steering (sys::peek buf ptr- :double)) (incf ptr- 8)
   ;; float64 _accel
     (setq _accel (sys::peek buf ptr- :double)) (incf ptr- 8)
   ;; float64 _brake
     (setq _brake (sys::peek buf ptr- :double)) (incf ptr- 8)
   ;;
   self)
  )

(setf (get autoku_msgs::VehicleInput :md5sum-) "78d4e62033b2c4038de1aac2b20b2457")
(setf (get autoku_msgs::VehicleInput :datatype-) "autoku_msgs/VehicleInput")
(setf (get autoku_msgs::VehicleInput :definition-)
      "float64 steering
float64 accel
float64 brake
")



(provide :autoku_msgs/VehicleInput "78d4e62033b2c4038de1aac2b20b2457")


