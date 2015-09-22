import scala.actors._
import java.util.BitSet;

// LAB 2: some case classes but you need additional ones too.

case class Start();
case class Stop();
case class Ready();
case class Go();
case class Change(in: BitSet);
case class Done();
case class UpdateNode();
case class Update(i:Int);
case class ChangeListed(moreWork:Boolean)


class Random(seed: Int) {
    var w = seed + 1;
    var z = seed * seed + seed + 2;

    def nextInt() = {
		z = 36969 * (z & 65535) + (z >> 16);
		w = 18000 * (w & 65535) + (w >> 16);

		(z << 16) + w;
	}
}

class Controller(val cfg: Array[Vertex]) extends Actor {
  var started = 0;
  val begin   = System.currentTimeMillis();
  var end = 0L;
  var isFinished:Boolean = false;
  var live:Int	= 0;
  var allStarted:Boolean = false;

  // LAB 2: The controller must figure out when
  //        to terminate all actors somehow.
  def act() {
    react {
    	case Ready() => {
        	started += 1;
			if (started == cfg.length) {
				for (u <- cfg) {
					u ! new Go;
				}
				allStarted = true;
        	}
        	act();
		}
		
		case Update(i) => {
			live += 1;
			sender ! new Go;
			act();
		}
		
		case Done() => {
// 			live -= 1;
			if(live == 0 && allStarted) {
				for (u <- cfg) {
					u ! new Stop;
				}	
				this ! new Stop;
				isFinished = true;
				end = System.currentTimeMillis();
				
				for (i <- 0 until cfg.length){
					cfg(i).print;
				}
				
				println("T = " + (end - begin)/1e9 + " s");
				
			} else {
				act(); 
 			}
      }

      case Stop() => {}
    }
  }
}

class Vertex(val index: Int, s: Int, val controller: Controller) extends Actor {
  var pred: List[Vertex] = List();
  var succ: List[Vertex] = List();
  val uses               = new BitSet(s);
  val defs               = new BitSet(s);
  var in                 = new BitSet(s);
  var out                = new BitSet(s);
  var listed:Boolean 	 = false;
  var count:Int 		 = 0;
			  
  def connect(that: Vertex) {
    println(this.index + "->" + that.index);
    this.succ = that :: this.succ;
    that.pred = this :: that.pred;
  }
  
  def act() { 
    react {
      	case Start() => {
	      controller ! new Ready;
	      act();
      	}
	  
		// LAB 2: Start working with this vertex.
		case Go() => {
			count += 1;
			update(true);
			act();
	 	}
	 	
	 	case Change(bs) => {
		 	listed = true;
		 	out.or(bs);
		 	update(true);
			act();
		}
	 	
	 	case Stop()  => {}
    }
    }
    
    def update(more_work: Boolean) {
	    val old = in;
		in = new BitSet();
		in.or(out);	
		in.andNot(defs);	
		in.or(uses);
		val i = index;
		if(!old.equals(in) || more_work) {
			for (p <- pred){
				val bs = new BitSet();
				bs.or(in);
				p ! new Change(bs);	
				controller ! new Update(i);
			}
		}	
		listed = false;
		controller ! new Done(); 
  	}

  def printSet(name: String, index: Int, set: BitSet) {
    System.out.print(name + "[" + index + "] = { ");
    for (i <- 0 until set.size)
      if (set.get(i))
        System.out.print("" + i + " ");
    println("}");
  }

  def print = {
    printSet("uses", index, uses);
    printSet("defs", index, defs);
    printSet("in", index, in);
    printSet("out", index, out);
    println("");
  }
}

object Driver {
  val rand    = new Random(1);
  var nactive = 0;
  var nsym    = 0;
  var nvertex = 0;
  var maxsucc = 0;

  def makeCFG(cfg: Array[Vertex]) {

    cfg(0).connect(cfg(1));
    cfg(0).connect(cfg(2));

    for (i <- 2 until cfg.length) {
      val p = cfg(i);
      val s = (rand.nextInt() % maxsucc) + 1;

      for (j <- 0 until s) {
        val k = cfg((rand.nextInt() % cfg.length).abs);
        p.connect(k);
      }
    }
  }

  def makeUseDef(cfg: Array[Vertex]) {
    for (i <- 0 until cfg.length) {
      for (j <- 0 until nactive) {
        val s = (rand.nextInt() % nsym).abs;
        if (j % 4 != 0) {
          if (!cfg(i).defs.get(s))
            cfg(i).uses.set(s);
        } else {
          if (!cfg(i).uses.get(s))
            cfg(i).defs.set(s);
        }
      }
    }
  }

  def main(args: Array[String]) {
    nsym           = args(0).toInt;
    nvertex        = args(1).toInt;
    maxsucc        = args(2).toInt;
    nactive        = args(3).toInt;
    val print      = args(4).toInt;
    val cfg        = new Array[Vertex](nvertex);
    val controller = new Controller(cfg);

    controller.start;

    println("generating CFG...");
    for (i <- 0 until nvertex)
      cfg(i) = new Vertex(i, nsym, controller);

    makeCFG(cfg);
    println("generating usedefs...");
    makeUseDef(cfg);

    println("starting " + nvertex + " actors...");

    for (i <- 0 until nvertex)
      cfg(i).start;

    for (i <- 0 until nvertex)
      cfg(i) ! new Start;
  }
}
