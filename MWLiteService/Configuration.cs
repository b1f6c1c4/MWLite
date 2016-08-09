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

        public bool UseTotalMines;
        public int TotalMines;
        public double Probability;

        public bool NotRigorous;

        // solver settings
        public bool DisableDual;
    }
}
