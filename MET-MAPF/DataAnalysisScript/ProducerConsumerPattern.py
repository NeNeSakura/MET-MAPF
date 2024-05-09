from multiprocessing import JoinableQueue,Process
import time,random



#生产者方法
def producer(name:str,tasks:iter,q:JoinableQueue):
    for i,task in enumerate(tasks) :
        time.sleep(random.randint(1, 2))
        f = 'Producer %s init the task with idx: %s'%(name,i)
        q.put(task)
        print(f)
    q.join()  #一直阻塞，等待消耗完所有的数据后才释放

#消费者方法
def consumer(name:str,q:JoinableQueue):
    while True:
        task = q.get()
        if task is None:
            break
        f= "Consumer %s execute the task."% (name)
        print(f)
        task()
        q.task_done() #每次消耗减1