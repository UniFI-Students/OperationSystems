if [ $# -eq 0 ]
	then
		echo "No arguments specified for the execution type."
		exit;
fi
gnome-terminal  --tab --title="CENTRAL ECU" -- bash -c "./hmi $1"; 
gnome-terminal  --tab --title="HMI WRITER"  -- bash -c "./hmi -w";
gnome-terminal  --tab --title="NETSTAT WATCHER" -- bash -c "watch -n 1 'netstat -nap | grep 127.0.0.1 | grep -v 'TIME_WAIT''";
