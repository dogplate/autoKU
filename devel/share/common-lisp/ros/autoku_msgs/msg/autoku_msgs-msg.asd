
(cl:in-package :asdf)

(defsystem "autoku_msgs-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils :geometry_msgs-msg
)
  :components ((:file "_package")
    (:file "LocalWaypointData" :depends-on ("_package_LocalWaypointData"))
    (:file "_package_LocalWaypointData" :depends-on ("_package"))
    (:file "VehicleInput" :depends-on ("_package_VehicleInput"))
    (:file "_package_VehicleInput" :depends-on ("_package"))
    (:file "VehicleOutput" :depends-on ("_package_VehicleOutput"))
    (:file "_package_VehicleOutput" :depends-on ("_package"))
    (:file "WaypointData" :depends-on ("_package_WaypointData"))
    (:file "_package_WaypointData" :depends-on ("_package"))
  ))