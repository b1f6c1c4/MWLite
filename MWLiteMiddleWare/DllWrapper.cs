using System;
using System.Linq;
using System.Runtime.InteropServices;

namespace MWLiteMiddleWare
{
    internal static class DllWrapper
    {
        [DllImport("MineSweeperAdapter.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr MakeWorker(RawConfiguration config, ulong repetition);

        [DllImport("MineSweeperAdapter.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr MakeWorkerT(RawConfiguration config, ulong repetition);

        [DllImport("MineSweeperAdapter.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr Run(IntPtr worker, [Out] out ulong len);

        [DllImport("MineSweeperAdapter.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void CancelWorker(IntPtr ptr);

        [DllImport("MineSweeperAdapter.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void DisposeWorker(IntPtr ptr);

        [DllImport("MineSweeperAdapter.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void DisposeResult(IntPtr ptr);

        public static IntPtr MakeWorker(Configuration config, ulong repetition, bool isT)
        {
            var decisionTree = config.DecisionTree.Select(m => (int)m).ToArray();
            var arr = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(int)) * decisionTree.Length);
            try
            {
                Marshal.Copy(decisionTree, 0, arr, decisionTree.Length);

                var cfg =
                    new RawConfiguration
                        {
                            Width = config.Width,
                            Height = config.Height,
                            TotalMines = config.TotalMines,
                            InitialPosition = config.InitialPosition,
                            Logic = config.Logic,
                            HeuristicEnabled = config.HeuristicEnabled,
                            DecisionTreeLen = config.DecisionTree.Count,
                            DecisionTree = arr,
                            ExhaustEnabled = config.ExhaustEnabled,
                            ExhaustCriterion = config.ExhaustCriterion
                        };

                return isT ? MakeWorkerT(cfg, repetition) : MakeWorker(cfg, repetition);
            }
            finally
            {
                Marshal.FreeHGlobal(arr);
            }
        }
    }
}
