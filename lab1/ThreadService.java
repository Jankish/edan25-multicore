import java.util.concurrent.*;
import java.util.LinkedList;
import java.util.ListIterator;

class ThreadService implements Runnable {

	static ThreadPoolExecutor executor;
	private static int maxThreads;
	public static final int MAX_SYSTEM_THREADS = Runtime.getRuntime().availableProcessors();
	private LinkedList<Vertex> worklist;
	private Runnable command;
	private static int threadCount;
	private int id;
	public static  long completedTaskCount;

	public ThreadService(LinkedList<Vertex> worklist, int nthread) {
		this.worklist = worklist;
		maxThreads = nthread;
		executor = (ThreadPoolExecutor) Executors.newFixedThreadPool(maxThreads);
		++threadCount;
		id = threadCount;
	}
	
	private ThreadService(LinkedList<Vertex> worklist) {
		this.worklist = worklist;
		++threadCount;
		id = threadCount;
	}
	public static void multiThreadLiveness(LinkedList<Vertex> worklist) {
		Vertex u;

		System.out.println("computing multi threaded liveness...");
		synchronized (worklist) {
			while (!worklist.isEmpty()) {
//				if(threadCount <= 1) {
//					executor.shutdown(); // POTENTIALLY DANGEROUS
////					if(executor.isTerminated()) {
////						executor = null;
////					}
////					completedTaskCount = executor.getCompletedTaskCount();
//					return;
//				}
//				
				u = worklist.remove();
				u.listed = false;
//				System.out.println("ThreadedLiveness " /*+ (id)*/ );
				u.computeIn(worklist);
			}
		}
		System.out.println("Pool Size -> " + executor.getPoolSize());
		if(threadCount >= maxThreads)
			multiThreadLiveness(worklist);
		else {
			ThreadService live = new ThreadService(worklist);
			try{
				executor.submit(live);
			} catch(Exception e){
				e.printStackTrace();
			}
//			multiThreadLiveness(worklist);
		}
		
		// for (i = 0; i < vertex.length; ++i)
		// vertex[i].print();
		
	}

	public void run() {
		// TODO Auto-generated method stub
		multiThreadLiveness(worklist);
	}
}
