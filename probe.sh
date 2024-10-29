memory_consumer_count=`ps -ef | grep -v grep | grep -c "memory-consumer"`

if [ ${memory_consumer_count} -lt 1 ]
then
	echo "memory-consumer not running"
	exit 1
fi
