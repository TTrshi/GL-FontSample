using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Diagnostics;
using System.Runtime.CompilerServices;

public class BitFlag
{
    uint[] flagReg;
    int flagRegLengthMax = 0;
    int flagRegArrayLength = 0;
    const int FLAG_BIT_SIZE = 32;
    public const int True = 1;
    public const int False = 0;

    public void Initialize(int flagLengthMax)
    {
        flagRegLengthMax = flagLengthMax;
        flagRegArrayLength = flagRegLengthMax / FLAG_BIT_SIZE + 1;
        flagReg = new uint[flagRegArrayLength];
        for(int i = 0; i< flagRegArrayLength; i++)
        {
            flagReg[i] = 0;
        }
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public void SetReg(int regno, bool b)
    {
        int index = regno / FLAG_BIT_SIZE;
        if (index >= flagRegArrayLength) return;
        int n = regno % FLAG_BIT_SIZE;
        if (b)
        {
            flagReg[index] |= (uint)(1 << n);
        }
        else
        {
            flagReg[index] &= ~(uint)(1 << n);
        }
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public bool GetReg(int regno)
    {
        int index = regno / FLAG_BIT_SIZE;
        if (index >= flagRegArrayLength) return false;
        int n = regno % FLAG_BIT_SIZE;
        uint bit = (uint)(1 << n);
        return (flagReg[index] & bit) == bit;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public int GetLengthMax()
    {
        return flagRegLengthMax;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public int SumOnCount()
    {
        int result = 0;
        for (int i = 0; i < flagRegArrayLength; i++)
        {
            uint value = flagReg[i];
            for (; value > 0; value >>= 1)
            {
                result += (int)(value & 1);
            }
        }
        return result;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public int SumOffCount()
    {
        return flagRegLengthMax - SumOnCount();
    }
}
