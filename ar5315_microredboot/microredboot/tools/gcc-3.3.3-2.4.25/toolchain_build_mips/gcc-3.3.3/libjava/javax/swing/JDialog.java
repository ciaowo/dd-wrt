/* JDialog.java --
   Copyright (C) 2002 Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */

package javax.swing;

import java.awt.*;
import java.awt.event.*;

import javax.accessibility.Accessible;
import javax.accessibility.AccessibleContext;
import javax.accessibility.AccessibleRole;
import javax.accessibility.AccessibleState;
import javax.accessibility.AccessibleStateSet;

/**
 * Unlike JComponent derivatives, JDialog inherits from
 * java.awt.Dialog. But also lets a look-and-feel component to its work.
 *
 * @author Ronald Veldema (rveldema@cs.vu.nl)
 */
public class JDialog extends Dialog implements Accessible
{
    public final static int HIDE_ON_CLOSE        = 0;
    public final static int DISPOSE_ON_CLOSE     = 1;
    public final static int DO_NOTHING_ON_CLOSE  = 2;

    protected  AccessibleContext accessibleContext;

    private int close_action = HIDE_ON_CLOSE;    

    /***************************************************
     *
     *
     *  constructors
     *
     *
     *************/

    JDialog(Frame owner)
    {
	this(owner, "dialog");
    }
    
    JDialog(Frame owner,
	    String s)
    {
	this(owner, s, true);
    }
    
  JDialog(Frame owner,
	  String s,
	  boolean modeld)
    {
	super(owner, s, modeld);
    }

  JDialog(Frame owner,
	  //  String s,
	  boolean modeld)
    {
	super(owner, "JDialog", modeld);
    }
  JDialog(Dialog owner)
  {
      this(owner, "dialog");
  }
    
    JDialog(Dialog owner,
	    String s)
    {
	this(owner, s, true);
    }
    
  JDialog(Dialog owner,
	  String s,
	  boolean modeld)
    {
	super(owner, s, modeld);
    }


    /***************************************************
     *
     *
     *  methods, this part is shared with JDialog, JFrame
     *
     *
     *************/

  
    private boolean checking;
    protected  JRootPane         rootPane;

    void setLocationRelativeTo(Component c)
    {
    }


    protected  void frameInit()
    {
      super.setLayout(new BorderLayout(1, 1));
      getRootPane(); // will do set/create
    }
  
  public Dimension getPreferredSize()
  {
    Dimension d = super.getPreferredSize();
    return d;
  }

    JMenuBar getJMenuBar()
    {    return getRootPane().getJMenuBar();   }
    
    void setJMenuBar(JMenuBar menubar)
    {    getRootPane().setJMenuBar(menubar); }
    

  public  void setLayout(LayoutManager manager)
  {    super.setLayout(manager);  }

    void setLayeredPane(JLayeredPane layeredPane) 
    {   getRootPane().setLayeredPane(layeredPane);   }
  
    JLayeredPane getLayeredPane()
    {   return getRootPane().getLayeredPane();     }
  
    JRootPane getRootPane()
    {
	if (rootPane == null)
	    setRootPane(createRootPane());
	return rootPane;          
    }

    void setRootPane(JRootPane root)
    {
	if (rootPane != null)
	    remove(rootPane);
	    
	rootPane = root; 
	add(rootPane, BorderLayout.CENTER);
    }

    JRootPane createRootPane()
    {   return new JRootPane();    }

    Container getContentPane()
    {    return getRootPane().getContentPane();     }

    void setContentPane(Container contentPane)
    {    getRootPane().setContentPane(contentPane);    }
  
    Component getGlassPane()
    {    return getRootPane().getGlassPane();   }
  
    void setGlassPane(Component glassPane)
    {   getRootPane().setGlassPane(glassPane);   }

    
    protected  void addImpl(Component comp, Object constraints, int index)
    {	super.addImpl(comp, constraints, index);    }


    public void remove(Component comp)
    {   getContentPane().remove(comp);  }
  
    protected  boolean isRootPaneCheckingEnabled()
    {    return checking;        }


    protected  void setRootPaneCheckingEnabled(boolean enabled)
    { checking = enabled;  }


    public void update(Graphics g)
    {   paint(g);  }

    protected  void processKeyEvent(KeyEvent e)
    {	super.processKeyEvent(e);    }

    /////////////////////////////////////////////////////////////////////////////////
  

    protected  void processWindowEvent(WindowEvent e)
    {
	//	System.out.println("PROCESS_WIN_EV-1: " + e);
	super.processWindowEvent(e); 
	//	System.out.println("PROCESS_WIN_EV-2: " + e);
	switch (e.getID())
	    {
	    case WindowEvent.WINDOW_CLOSING:
		{
		    switch(close_action)
			{
			case DISPOSE_ON_CLOSE:
			    {
				System.out.println("user requested dispose on close");
				dispose();
				break;
			    }
			case HIDE_ON_CLOSE:
			    {
				setVisible(false);
				break;
			    }
			case DO_NOTHING_ON_CLOSE:
			    break;
			}
		    break;
		}
		
	    case WindowEvent.WINDOW_CLOSED:
	    case WindowEvent.WINDOW_OPENED:
	    case WindowEvent.WINDOW_ICONIFIED:
	    case WindowEvent.WINDOW_DEICONIFIED:
	    case WindowEvent.WINDOW_ACTIVATED:
	    case WindowEvent.WINDOW_DEACTIVATED:
		break;
	    }
    }   
 

    void setDefaultCloseOperation(int operation)
    {  close_action = operation;   }

    protected  String paramString()
    {   return "JDialog";     }

    public AccessibleContext getAccessibleContext()
    {
	return null;
    }  
}
