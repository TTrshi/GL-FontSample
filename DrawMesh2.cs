using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using System.Runtime.CompilerServices;
using System;

public class DrawMesh2 : MonoBehaviour
{
    private List<int> _triangles = new List<int>();
    private Vector3[] _vertices = null;
    private int _verticesLength = 0;
    BitFlag _bitFlag = new BitFlag();

    private bool _isError = false;
    private int _curIndexLast = -1;

    private bool _isIncluding = false;
    private int _curIndex;
    private int _nextIndex;
    private int _prevIndex;

    private Vector3 CurrentPoint
    {
        get { return _vertices[_curIndex]; }
    }
    private Vector3 PreviousPoiont
    {
        get { return _vertices[_prevIndex]; }
    }
    private Vector3 NextPoint
    {
        get { return _vertices[_nextIndex]; }
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    private void Initialize(List<Vector3> vertices)
    {
        Clear();

        // ê›íËÇ≥ÇÍÇΩí∏ì_Çï€éùÇµÇƒÇ®Ç≠
        _vertices = vertices.ToArray();
        _verticesLength = vertices.Count;

        _bitFlag.Initialize(_verticesLength);

        _isError = false;
        _curIndexLast = -1;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    private void Clear()
    {
        if (_vertices != null && _vertices.Length != 0)
        {
            Array.Clear(_vertices, 0, _vertices.Length);
            Array.Resize(ref _vertices, 0);
        }
        _triangles.Clear();
        _bitFlag.Clear();
    }

    public bool CreateMesh(List<Vector3> vertices, ref int[] triangles)
    {
        Initialize(vertices);

        while (_bitFlag.SumOffCount() > 3)
        {
            if(_isError)
            {
                Clear();
                return false;
            }
            DetecteTriangle();
        }

        for (int i = 0; i < _bitFlag.GetLengthMax(); i++)
        {
            if (!_bitFlag.GetReg(i))
            {
                _bitFlag.SetReg(i, true);
                _triangles.Add(i);
            }
        }

        triangles = _triangles.ToArray();
        Clear();
        return true;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    private void DetecteTriangle()
    {
        if (!_isIncluding)
        {
            FindFarPoint();
        }

        Vector3 edge1 = NextPoint - CurrentPoint;
        Vector3 edge2 = CurrentPoint - PreviousPoiont;
        float dot_ = Vector3.Dot(edge1.normalized, edge2.normalized);
        if (dot_ > 0.99f)
        {
            _isIncluding = false;
            _bitFlag.SetReg(_curIndex, true);
            return;
        }

        if (IsIncludePoint())
        {
            _isIncluding = true;
            MoveToNext();
            if (_curIndex == _curIndexLast) _isError = true;
            return;
        }

        _isIncluding = false;

        _triangles.Add(_curIndex);
        _triangles.Add(_nextIndex);
        _triangles.Add(_prevIndex);

        _bitFlag.SetReg(_curIndex, true);
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    private bool IsIncludePoint()
    {
        for (int i = 0; i < _bitFlag.GetLengthMax(); i++)
        {
            int index = i;

            if (_bitFlag.GetReg(i))
            {
                continue;
            }

            // skip if index in detected three points.
            if (index == _curIndex || index == _nextIndex || index == _prevIndex)
            {
                continue;
            }

            if (CheckInPoint(_vertices[index]))
            {
                return true;
            }
        }

        return false;
    }


    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    private bool CheckInPoint(Vector3 target)
    {
        // Triangle points.
        Vector3[] tp =
        {
            CurrentPoint,
            NextPoint,
            PreviousPoiont,
        };

        Vector3 prevNormal = default(Vector3);
        for (int i = 0; i < tp.Length; i++)
        {
            Vector3 edge1 = (target - tp[i]);
            Vector3 edge2 = (target - tp[(i + 1) % tp.Length]);

            Vector3 normal = Vector3.Cross(edge1, edge2).normalized;

            if (prevNormal == default(Vector3))
            {
                prevNormal = normal;
                continue;
            }

            // If not same direction, the point out of a triangle.
            if (Vector3.Dot(prevNormal, normal) <= 0.99f)
            {
                return false;
            }
        }

        return true;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    private void MoveToNext()
    {
        _curIndex = FindNextIndex(_curIndex);
        _nextIndex = FindNextIndex(_curIndex);
        _prevIndex = FindPrevIndex(_curIndex);
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    private void FindFarPoint()
    {
        int farIndex = -1;
        float maxDist = float.MinValue;

        for (int i = 0; i < _bitFlag.GetLengthMax(); i++)
        {
            if (_bitFlag.GetReg(i))
            {
                continue;
            }

            float dist = (_vertices[i] - Vector3.zero).sqrMagnitude;
            if (dist > maxDist)
            {
                maxDist = dist;
                farIndex = i;
            }
        }

        _curIndex = farIndex;
        _nextIndex = FindNextIndex(_curIndex);
        _prevIndex = FindPrevIndex(_curIndex);

        _curIndexLast = _curIndex;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    private int FindNextIndex(int start)
    {
        int i = start;
        int prevStart = (i + 1) % _verticesLength;
        do
        {
            i = (i + 1) % _verticesLength;
            if (!_bitFlag.GetReg(i))
            {
                return i;
            }
        } while (start != prevStart);

        _isError = true;
        return -1;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    private int FindPrevIndex(int start)
    {
        int i = start;
        int prevStart = (i - 1) >= 0 ? i - 1 : _verticesLength - 1;
        do
        {
            i = (i - 1) >= 0 ? i - 1 : _verticesLength - 1;
            if (!_bitFlag.GetReg(i))
            {
                return i;
            }
        } while (start != prevStart);

        _isError = true;
        return -1;
    }
}