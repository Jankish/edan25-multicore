import java.util.concurrent.*;
import java.util.LinkedList;
import java.util.ListIterator;

class ThreadService implements Runnable {

	static ExecutorService executor;
	private int nThread;
	private LinkedList<Vertex> worklist;

	public ThreadService(LinkedList<Vertex> worklist, int nthread) {
		nThread = nthread;
		this.worklist = worklist;
		executor = Executors.newFixedThreadPool(nThread);
	}

	public static void multiThreadLiveness(LinkedList<Vertex> worklist) {
		Vertex u;

		System.out.println("computing multi threaded liveness...");
		synchronized (worklist) {
			while (!worklist.isEmpty()) {
				u = worklist.remove();
				u.listed = false;
				u.computeIn(worklist);
			}
		}

		// for (i = 0; i < vertex.length; ++i)
		// vertex[i].print();
	}

	@Override
	public void run() {
		// TODO Auto-generated method stub
		multiThreadLiveness(worklist);
	}
}
