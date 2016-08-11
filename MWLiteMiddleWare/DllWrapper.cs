using System;
using System.Runtime.InteropServices;

namespace MWLiteMiddleWare
{
    internal static class DllWrapper
    {
        [DllImport("MWLite.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr MakeWorker(Configuration config, LogicLevel level, ulong repetition);

        [DllImport("MWLite.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr Run(IntPtr worker, [Out] out ulong len);

        [DllImport("MWLite.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void CancelWorker(IntPtr ptr);

        [DllImport("MWLite.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void DisposeWorker(IntPtr ptr);

        [DllImport("MWLite.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void DisposeResult(IntPtr ptr);
    }
}
