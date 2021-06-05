package merkletree;

import java.security.MessageDigest;

/**
 * Represents a binary Merkle Tree. This consists of two child nodes, and a
 * hash representing those two child nodes. The children can either be leaf nodes
 * that contain data blocks, or can themselves be Merkle Trees.
 */
public class MerkleTree
{
	// Child trees
	public MerkleTree leftTree = null;
    public MerkleTree rightTree = null;


	public MerkleTree parentTree = null;
	public int parentPos = 0; //0:left 1:right

    // The hash value of this node
	public byte[] digest = null;

	// The digest algorithm
	public MessageDigest md;


    /**
     	 * Initialises an empty Merkle Tree using the specified
     	 * digest algorithm.
     	 *
     	 * @param md The message digest algorithm to be used by the tree
     	 */
	public MerkleTree(MessageDigest md) { this.md = md; }

    public boolean isLeaf()
    {
        return this instanceof Leaf;
    }

    public void add(MerkleTree leftTree, MerkleTree rightTree)
    {
        this.leftTree = leftTree;
        this.rightTree = rightTree;
    }

    public void updateDigest(MerkleTree leftTree, MerkleTree rightTree)
    {
        add(leftTree,rightTree);
        if(leftTree != null && rightTree != null)
        {
            md.update(leftTree.digest);
            digest = md.digest(rightTree.digest);
        }
        else if(leftTree == null && rightTree != null)
        {
            digest = md.digest(rightTree.digest);
        }
        else if(leftTree != null && rightTree == null)
        {
            digest = md.digest(leftTree.digest);
        }
    }


    public void setTree(MerkleTree tree,int pos)
    {
        if(pos == 0)
            this.leftTree = tree;
        else
            this.rightTree = tree;
    }

    /**
	 * Returns a string representation of the specified
	 * byte array, with the values represented in hex. The
	 * values are comma separated and enclosed within square
	 * brackets.
	 *
	 * @param array The byte array
	 *
	 * @return Bracketed string representation of hex values
	 */
	private String toHexString(byte[] array)
	{
		final StringBuilder str = new StringBuilder();

		str.append("[");

		boolean isFirst = true;
        for (final byte b : array) {
            if (isFirst) {
                //str.append(Integer.toHexString(i));
                isFirst = false;
            } else {
                //str.append("," + Integer.toHexString(i));
                str.append(",");
            }

            final int hiVal = (b & 0xF0) >> 4;
            final int loVal = b & 0x0F;
            str.append((char) ('0' + (hiVal + (hiVal / 10 * 7))));
            str.append((char) ('0' + (loVal + (loVal / 10 * 7))));
        }

		str.append("]");

		return(str.toString());
	}

	/**
	 * prettyPrint in which the number
	 * of spaces to indent the tree are specified
	 *
	 * @param indent The number of spaces to indent
	 */
	public void prettyPrint(int indent)
	{
		for(int idx=0; idx<indent; idx++)
		{
			System.out.print(" ");
		}

		// Print root digest
		System.out.println("Node digest: " + toHexString(digest));
		if(leftTree != null)
        {
            leftTree.prettyPrint(indent + 1);
        }
		else
        {
            System.out.println("Left Tree is Empty");
        }
		if(rightTree != null)
        {
            rightTree.prettyPrint(indent + 1);
        }
        else
        {
            System.out.println("Right Tree is Empty");
        }
	}

	/**
	 * Formatted print out of the contents of the tree
	 */
	public void prettyPrint()
	{
		// Pretty print the tree, starting with zero indent
		prettyPrint(0);
	}
}
