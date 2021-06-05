package merkletree;

import java.security.MessageDigest;

/**
 * Represents a Merkle Tree leaf, consisting of a list
 * of blocks of arbitrary data. The arbitrary data in each block
 * is represented as a byte array.
 */
public class Leaf extends MerkleTree
{
	// The data to be stored in this node
	private byte[] dataBlock;
	public int blkNum;
	public String fileName;



    /**
	 * Initialises the leaf node, which consists
	 * of the specified block of data.
	 *
	 * @param dataBlock Data block to be placed in the leaf node
	 */
	public Leaf(MessageDigest md,byte[] dataBlock, String fileName)
	{
        super(md);
        this.blkNum = Integer.parseInt(fileName.substring(fileName.length()-1));
	    this.dataBlock = dataBlock;
	    this.fileName = fileName;
	}



    /**
	 * @return The data block associated with this leaf node
	 */
	public byte[] getDataBlock()
	{
		return (dataBlock);
	}


    public void updateDigest()
    {
        this.digest = md.digest(dataBlock);
    }

    /**
     * prettyPrint in which the number
     * of spaces to indent the tree are specified
     *
     * @param indent The number of spaces to indent
     */
    public void prettyPrint(int indent)
    {
        if(digest != null)
        {
            for(int idx=0; idx<indent; idx++) {
				System.out.print(" ");
			}
            String position = (parentPos == 0) ? "Leaf" : "Right";
            System.out.println(position + " leaf: File " + fileName + ":" + this.toString());
        }
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
		StringBuilder str = new StringBuilder();

		str.append("[");

		boolean isFirst = true;
		for(int idx=0; idx<array.length; idx++)
		{
			byte b = array[idx];

			if (isFirst)
			{
				//str.append(Integer.toHexString(i));
				isFirst = false;
			}
			else
			{
				//str.append("," + Integer.toHexString(i));
				str.append(",");
			}

			int hiVal = (b & 0xF0) >> 4;
	        int loVal = b & 0x0F;
	        str.append((char) ('0' + (hiVal + (hiVal / 10 * 7))));
	        str.append((char) ('0' + (loVal + (loVal / 10 * 7))));
		}

		str.append("]");

		return(str.toString());
	}

	/**
	 * Returns a string representation of the data block
	 */
	public String toString()
	{
		StringBuilder str = new StringBuilder();

		str.append(toHexString(dataBlock));

		return(str.toString());
	}

}
