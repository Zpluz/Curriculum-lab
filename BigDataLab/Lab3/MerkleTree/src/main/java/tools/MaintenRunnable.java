package tools;

import merkletree.TreeMaintenancer;

public class MaintenRunnable implements Runnable{
    private TreeMaintenancer tm = null;
    public MaintenRunnable(TreeMaintenancer tm)
    {
        this.tm = tm;
    }

    @Override
    public void run() {
        System.out.println("Maintenance program has initialized");
        while (true)
        {
            if(tm.mainTain() == 1)
            {
                System.out.println("Files in "+ tm.dirPath +" have been modified");
                System.out.println("Modified Tree:");
                tm.mt.prettyPrint();
                System.out.println("\n");
            }
            else
            {
                System.out.println("Files in "+ tm.dirPath +" haven't been modified");
            }
            System.out.println("Waiting for 10 seconds to examine\n");
            try {
                Thread.sleep(10_000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
