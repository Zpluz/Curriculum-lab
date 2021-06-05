package merkletree;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import tools.MaintenRunnable;

public class Startup {
    public static void main(String[] args) {

        //Modify
        String DirPath1 = "C:\\Users\\Zilai Zeng\\source\\course\\BigDataLab\\Lab3\\MerkleTree\\src\\main\\resources\\Blockset1\\";
        MessageDigest md = null;
        try
        {
            md = MessageDigest.getInstance("SHA");
        }
        catch (NoSuchAlgorithmException e)
        {
            // Should never happen, we specified SHA, a valid algorithm
            assert false;
        }
        TreeBuilder tbm = new TreeBuilder(DirPath1,md);
        MerkleTree mt = tbm.Treebuild();
        System.out.println("Original Tree:");
        mt.prettyPrint();
        System.out.println("\n");

        //Modify
//        TreeMaintenancer tm = new TreeMaintenancer(DirPath1,md,mt);
//        Thread maintenance = new Thread(new MaintenRunnable(tm));
//        maintenance.start();


        //Compare
        System.out.println("\n");
        String DirPath3 = "C:\\Users\\Zilai Zeng\\source\\course\\BigDataLab\\Lab3\\MerkleTree\\src\\main\\resources\\Blockset3\\";
        TreeBuilder tbs = new TreeBuilder(DirPath3,md);
        MerkleTree st = tbs.Treebuild();
        System.out.println("Sub Tree:");
        st.prettyPrint();
        System.out.println("\n");
        TreeComparer tc = new TreeComparer(mt,st,DirPath1,DirPath3);
        int flag = tc.getComparison();
        if(flag != 1)
        {

            System.out.println("Comparison Completed, Two trees fail matching, synchronize files and rebuild sub tree");
            if(flag == -1)
            {
                System.out.println("Synchronizing All...");
                tc.SynchronizeAll();
            }
            else
            {
                System.out.println("Synchronizing...");
                tc.Synchronize();
            }
            System.out.println("Synchronize Complete");
            System.out.println("Rebuilding...");
            MerkleTree stR = tbs.Treebuild();
            System.out.println("Rebuilding Complete\n");
            System.out.println("Rebuild Sub Tree:");
            stR.prettyPrint();
            System.out.println("\n");
            TreeComparer tcR = new TreeComparer(mt,stR,DirPath1,DirPath3);
            System.out.println("Compare Again");
            int flagR = tcR.getComparison();
            if(flagR == 1)
            {
                System.out.println("Comparison Completed, Two trees succeed matching, synchronize succeed\n");
            }
            else
            {
                System.out.println("Rebuilding Fail");
            }
        }
        else
            System.out.println("Comparison Completed, Two trees succeed matching\n");

    }
}
