# What is this?

This is a small example that shows that the pod status may be misleading. 
When one of the processes in a multi-process container gets killed, the container may keep on running.
However, when the "main" process in the container is killed, the pod will be marked as `OOMKilled`, even though the OOM kill wasn't the actual reason why the container terminated.

# Steps to reproduce

1. Deploy the pod 

   ```bash
   kubectl apply -f https://raw.githubusercontent.com/luksa/oom-in-multi-process-container/refs/heads/master/multi-process-container-noprobe.yaml
   ```

   Wait for the pod to be running.

2. Send two requests to the pod

   ```bash
   kubectl exec multi-process-container -- curl -s http://localhost:8080
   kubectl exec multi-process-container -- curl -s http://localhost:8080
   ```

   This will cause the container to exceed its memory limit, causing the `memory-consumer` process to be OOMKilled. 
   The other process in the container (`http-server`) will continue to run.

3. Confirm the OOM kill

   Check the `dmesg` log on the pod's host node. Look for the following line:
   
   ```
   Memory cgroup out of memory: Killed process 646104 (memory-consumer) total-vm:84028kB, anon-rss:81972kB, file-rss:1576kB, shmem-rss:0kB, UID:0 pgtables:196kB oom_score_adj:-997
   ```

4. Check the pod's status to confirm that it's still running

   ```bash
   $ kubectl get pods
   NAME                      READY   STATUS    RESTARTS   AGE
   multi-process-container   1/1     Running   0          15s
   ```
   
   The pod is still running, even though one of the processes was OOMKilled.

5. Kill the http-server container in the pod

   Wait for however long you want, then kill the `http-server` process in the container:
   
   ```bash
   kubectl exec multi-process-container -- pkill -f "http-server"
   ```

6. Check the pod's status again

   ```bash
   $ kubectl get pods
   NAME                      READY   STATUS      RESTARTS   AGE
   multi-process-container   0/1     OOMKilled   0          25s
   ```
   
   Notice that the pod is in the `OOMKilled` state, even though the `http-server` process was killed by the user, not by the OOM killer.
   Note: if the container has already been restarted, then the status will already have changed to `Running`. To see the `OOMKilled` status, check the YAML:
   ```bash
   $ kubectl get pod multi-process-container -oyaml | yq .status.containerStatuses
   - containerID: cri-o://d65beb78a5f984e170e21a8b909978532686b1b6903cfc429a3023d75308dbf7
     image: quay.io/luksa/multi-process-container:latest
     imageID: quay.io/luksa/multi-process-container@sha256:2b98e47ee87c040743b551f27206d86b4ee3ac7d262c2827baeb2d2ff2661799
     lastState:
       terminated:
         containerID: cri-o://4de360ee2cd2fc1d2c7ee7766f379e2fe445eebcccfa713d8268971314b8a274
         exitCode: 143
         finishedAt: "2024-10-29T10:33:25Z"
         reason: OOMKilled
         startedAt: "2024-10-29T10:31:53Z"
     name: main
     ready: true
     restartCount: 1
     started: true
     state:
       running:
         startedAt: "2024-10-29T10:33:28Z"
   ```

# Further examples

1. Instead of killing the `http-server` process, you can also see what happens when a liveness probe is configured for the container. 
Use the `multi-process-container.yaml` file to see this in action.
Here, the `memory-consumer` process is OOMkilled, and then, 30s later, the container is restarted because the liveness probe fails. 
The status of the pod will be `OOMKilled`, even though the container was running fine after the OOM kill and was actually restarted because of the liveness probe failure.

2. The outcome is the same if the liveness probe times out. Use the `multi-process-container-probetimeout.yaml` file to see this in action.