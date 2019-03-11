import java.util.*;
import java.io.*;
public class TestJni
{
	//for test
	public native void print(String text);
	//Native方法，输出到屏幕
	private static native boolean printTextNative(String text);
	//按照行列输入到屏幕
	private static native boolean printTextWithPosNative(int x, int y, char text);
	//屏幕心跳
	//private static native boolean printHeart(int fd);
	//修改全部led颜色
	private native boolean changeAllLedStatus(int[] states);
	//修改单个led颜色
	private native boolean changeLedStatus(int x, int y, int c);
	//修改全部通道状态
	public native boolean changeAllChannelStatus(String vals, String time, int ex_ch); 
	//修改单个通道状态
	public native boolean changeChannelStatus(int num, int state, int ex_ch);
	//获取当前所有通道状态
	private static native int[] getChannelStatusNative(int num);
	//获取当前通道状态
	private static native int getChannelNumNative();
	//获取强电回路驱动信息
	private static native String[] getCircuitNative(int ex_ch);
	//获取GPS当前状态
	private static native String[] getGPSStatusNative(int ex_ch);
	//获取光照强度
	private static native String getLightIntensityNative(int ex_ch);
	//获取电流电压
	private static native String getCurrentAndVoltageNative(int ex_ch);
	//获取功率
	private static native String[] getPowerNative(int ex_ch);
	//蜂鸣器：1 打开； 0 关闭
	private static native boolean BuzzerNative(int val);

	static
	{
		System.loadLibrary("TestJni");
	}

	public static void main(String[] args)
		throws Exception
		//throws java.io.UnsupportedEncodingException
		{
			TestJni test = new TestJni();
			int val = 1;
			//int[] vals = new int[] {0, 0, 0};
			//int[] led_vals = new int[] {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}; //red
			//int[] led_vals = new int[] {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2}; //green
			//int[] led_vals = new int[] {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3}; //blue
			int[] led_vals = new int[] {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4}; //


			//for test
			test.print("hello!");


			test.changeAllChannelStatus("1111111111111111", "00050", 0);

			//test.printTextNative("Welcome PAVT$P437x Board$Version1.0$Happyfrog");
			//while(val < 10000)
			//{
			//	test.printTextWithPosNative(8, 0, 'P');
			//	Thread.sleep(500);
			//	test.printTextWithPosNative(8, 0, ' ');
			//	Thread.sleep(500);
			//}

			//test.changeAllLedStatus(led_vals);
			//test.changeLedStatus(6, 0, 3);
			//val = test.getChannelNumNative();
			//int[] vals = test.getChannelStatusNative(8);
			//System.out.println("channel_statu:" + val);
			//for(int i = 0; i < vals.length; i++)
			//	System.out.println(vals[i]);

			//String gps_vals[] = test.getGPSStatusNative(0);
			//for(int i = 0; i < gps_vals.length; i++)
			//{
			//	gps_vals[i] = new String(gps_vals[i].getBytes("ISO8859-1"), "GBK");
			//	System.out.println(gps_vals[i]);
			//}

			//String lux_val = test.getLightIntensityNative(0);
			//lux_val = new String(lux_val.getBytes("ISO8859-1"), "GBK");
			//System.out.println(lux_val);

			//String power_vals[] = test.getPowerNative(0);
			//for(int i = 0; i < power_vals.length; i++)
			//{
			//	power_vals[i] = new String(power_vals[i].getBytes("ISO8859-1"), "GBK");
			//	System.out.println(power_vals[i]);
			//}

			//String cir_vals[] = test.getCircuitNative(0);
			//for(int i = 0; i < cir_vals.length; i++)
			//{
			//	cir_vals[i] = new String(cir_vals[i].getBytes("ISO8859-1"), "GBK");
			//	System.out.println(cir_vals[i]);
			//}

			//test.BuzzerNative(1);
			//test.BuzzerNative(1);
			//Thread.sleep(3000);

			//test.BuzzerNative(0);
	}
}
