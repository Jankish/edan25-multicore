import java.util.LinkedList;

public class WorkService implements Runnable{
	private static int maxThreads;
	private LinkedList<Vertex> worklist;
	private static int threadCount;
	private int id;
	static int completedTasks;
	Vertex u;

	public WorkService(LinkedList<Vertex> worklist) {
		this.worklist = worklist;
		

	}

	@Override
	public void run() {
		while(!worklist.isEmpty()) {
			u = worklist.remove();
			u.listed = false;
			u.computeIn(worklist);
		}
	}
}
