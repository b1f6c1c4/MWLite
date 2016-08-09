using System.Runtime.InteropServices;

namespace MWLiteService
{
    public enum LogicLevel
    {
        ZeroLogic = 0,
        PassiveLogic = 1,
        SingleLogic = 2,
        SingleLogicExtended = 3,
        DoubleLogic = 4,
        DoubleLogicExtended = 5,
        FullLogic = 6
    };

    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    public struct Configuration
    {
        // BaseConfiguration
        public int Width;
        public int Height;

        [MarshalAs(UnmanagedType.U1)] public bool UseTotalMines;
        public int TotalMines;
        public double Probability;

        [MarshalAs(UnmanagedType.U1)] public bool NotRigorous;

        // Configuration
        [MarshalAs(UnmanagedType.U1)] public LogicLevel Logic;
    }
}
