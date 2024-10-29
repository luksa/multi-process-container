build:
	docker build -t quay.io/luksa/multi-process-container:latest .

push: build
	docker push quay.io/luksa/multi-process-container:latest

deploy:
	kubectl apply -f multi-process-container.yaml

deploy-no-probe:
	kubectl apply -f multi-process-container-noprobe.yaml

deploy-probe-timeout:
	kubectl apply -f multi-process-container-probe-timeout.yaml

kill-http-server:
	kubectl exec multi-process-container -- pkill -f "http-server"

undeploy:
	kubectl delete -f multi-process-container.yaml

docker-run:
	docker run -it -m 100M --rm --name multi-process-container quay.io/luksa/multi-process-container:latest

watch-memory:
	kubectl exec -it multi-process-container -- watch -n 1 "cat /sys/fs/cgroup/memory.current /sys/fs/cgroup/memory.max"

request:
	kubectl exec multi-process-container -- curl -s http://localhost:8080