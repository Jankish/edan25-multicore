import java.util.concurrent.*;
import java.util.LinkedList;
import java.util.ListIterator;

class ThreadService implements Executor{

	static ThreadPoolExecutor executor;
	private int nThread;
	public static final int MAX_SYSTEM_THREADS = Runtime.getRuntime().availableProcessors();
	private LinkedList<Vertex> worklist;
	private Runnable command;

	public ThreadService(LinkedList<Vertex> worklist, int nthread) {
		this.worklist = worklist;
		executor = (ThreadPoolExecutor) Executors.newFixedThreadPool(nThread);
	}

	public static void multiThreadLiveness(LinkedList<Vertex> worklist) {
		Vertex u;

		System.out.println("computing multi threaded liveness...");
		synchronized (worklist) {
			while (!worklist.isEmpty()) {
				u = worklist.remove();
				u.listed = false;
//				System.out.println("ThreadedLiveness " /*+ (id)*/ );
				u.computeIn(worklist);
			}
		}

		// for (i = 0; i < vertex.length; ++i)
		// vertex[i].print();
	}

	public void run() {
		// TODO Auto-generated method stub
		execute(command);
	}

	@Override
	public void execute(Runnable command) {
		// TODO Auto-generated method stub
		this.command = command;
		multiThreadLiveness(worklist);
	}
}
