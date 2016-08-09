using System.Runtime.InteropServices;

// ReSharper disable FieldCanBeMadeReadOnly.Local

namespace MWLiteService
{
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    public struct Configuration
    {
        // generator settings
        public int Width;
        public int Height;

        [MarshalAs(UnmanagedType.U1)]
        public bool UseTotalMines;
        public int TotalMines;
        public double Probability;

        [MarshalAs(UnmanagedType.U1)]
        public bool NotRigorous;

        // solver settings
        [MarshalAs(UnmanagedType.U1)]
        public bool DisableDual;
    }
}
