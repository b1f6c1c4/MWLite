using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace MWLiteMiddleWare
{
    internal enum LogicMethod
    {
        Passive = 0x00,
        Single = 0x01,
        SingleExtended = 0x02,
        Double = 0x03,
        DoubleExtended = 0x04,
        Full = 0x05
    };

    internal enum HeuristicMethod
    {
        None = 0x00,
        MinMineProb = 0x01,
        MaxZeroProb = 0x02,
        MaxZerosProb = 0x03,
        MaxZerosExp = 0x04,
        MaxQuantityExp = 0x05,
        MinFrontierDist = 0x06,
        MaxUpperBound = 0x07
    };

    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    internal struct RawConfiguration
    {
        public int Width;
        public int Height;

        public int TotalMines;

        public int InitialPosition;

        public LogicMethod Logic;

        [MarshalAs(UnmanagedType.U1)] public bool HeuristicEnabled;

        public IntPtr DecisionTree;
        public int DecisionTreeLen;

        public bool ExhaustEnabled;
        public int ExhaustCriterion;
    }

    internal struct Configuration
    {
        public int Width;
        public int Height;

        public int TotalMines;

        public int InitialPosition;

        public LogicMethod Logic;

        public bool HeuristicEnabled;

        public List<HeuristicMethod> DecisionTree;

        public bool ExhaustEnabled;
        public int ExhaustCriterion;
    }

    internal struct WorkingConfig
    {
        public Configuration Config;
        public ulong Repetition;
    }
}
