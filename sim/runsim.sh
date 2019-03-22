SIM_DIR=$(cd "$(dirname ${BASH_SOURCE[0]})" && pwd)
MODEL_DIR=${SIM_DIR}/models:/.gazebo/models:/usr/share/gazebo-9/models/
PLUGIN_DIR=${SIM_DIR}/lib:/usr/lib/x86_64-linux-gnu/gazebo-9/plugins/
WORLD_DIR=${SIM_DIR}/worlds
PX4_MODEL_DIR=${SIM_DIR}/../thirdparty/Firmware/Tools/sitl_gazebo/models
PX4_PLUGIN_DIR=${SIM_DIR}/../thirdparty/Firmware/build/px4_sitl_default/build_gazebo

export LD_LIBRARY_PATH=${SIM_DIR}/../thirdparty/Firmware/build/posix_sitl_default/build_gazebo:$LD_LIBRARY_PATH
export GAZEBO_MODEL_PATH=$MODEL_DIR:$PX4_MODEL_DIR
export GAZEBO_PLUGIN_PATH=$PLUGIN_DIR:$PX4_PLUGIN_DIR

export PX4_HOME_LAT=42.29442158
export PX4_HOME_LON=-83.71037386
export PX4_HOME_ALT=269

echo "Model Path: $GAZEBO_MODEL_PATH"
echo "Plugin Path: $GAZEBO_PLUGIN_PATH"
echo "Resource Path $GAZEBO_RESOURCE_PATH"	

# Kill any remaining processes
pkill -x gazebo || true
pkill -x px4 || true
pkill -x gzserver || true
pkill -x gzclient || true

gzserver -s ${SIM_DIR}/lib/libSensorRegisterPlugin.so --verbose ${WORLD_DIR}/test.world &
SIM_PID=`echo $!`

if [[ -n "$HEADLESS" ]]; then
	echo "not running gazebo gui"
else
	# gzserver needs to be running to avoid a race. Since the launch
	# is putting it into the background we need to avoid it by backing off
	sleep 3
	nice -n 20 gzclient --verbose &
	GUI_PID=`echo $!`
fi

# Run PX4 flight controller
${SIM_DIR}/../thirdparty/Firmware/build/posix_sitl_default/px4 \
${SIM_DIR}/../thirdparty/Firmware \
${SIM_DIR}/../thirdparty/Firmware/posix-configs/SITL/init/ekf2/iris

# # Run PX4 flight controller
# ${SIM_DIR}/../thirdparty/Firmware/build/px4_sitl_default/bin/px4 \
# ${SIM_DIR}/../thirdparty/Firmware/ROMFS/px4fmu_common -s ${SIM_DIR}/../thirdparty/Firmware/ROMFS/px4fmu_common/init.d-posix/rcS -t


# # ${SIM_DIR}/../thirdparty/Firmware/test_data