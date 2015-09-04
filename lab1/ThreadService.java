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
	
	public static void multiThreadLiveness(LinkedList<Vertex> worklist)
	{
		Vertex			u;
		Vertex			v;
		int			i;
		LinkedList<Vertex>	worklist;
		long			begin;
		long			end;

		System.out.println("computing liveness...");

		begin = System.nanoTime();
		worklist = new LinkedList<Vertex>();

		for (i = 0; i < vertex.length; ++i) {
			worklist.addLast(vertex[i]);
			vertex[i].listed = true;
		}

		while (!worklist.isEmpty()) {
			u = worklist.remove();
			u.listed = false;
			u.computeIn(worklist);
		}
		end = System.nanoTime();

		System.out.println("T = " + (end-begin)/1e9 + " s");
//		for (i = 0; i < vertex.length; ++i) 
//			vertex[i].print();
	}

	@Override
	public void run() {
		// TODO Auto-generated method stub
		multiThreadLiveness(worklist);
	}
}	
