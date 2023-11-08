using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using System;

/// <summary>
/// Draw mesh by clicked points.
/// </summary>
public class DrawMesh : MonoBehaviour
{
    private List<int> _triangles = new List<int>();
    private List<Vector3> _vertices = new List<Vector3>();
    private Dictionary<int, bool> _verticesBuffer = new Dictionary<int, bool>();

    private Vector3 _prevDirection = Vector3.zero;

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

    /// <summary>
    /// Clear buffers.
    /// </summary>
    private void Clear()
    {
        _vertices.Clear();
        _verticesBuffer.Clear();
        _triangles.Clear();
    }

    private void Initialize(List<Vector3> vertices)
    {
        Clear();

        // 設定された頂点を保持しておく
        _vertices.AddRange(vertices);

        // 全頂点のインデックスを保持、使用済みフラグをfalseで初期化
        for (int i = 0; i < vertices.Count; i++)
        {
            _verticesBuffer.Add(i, false);
        }
    }

    /// <summary>
    /// Create mesh by vertices.
    /// </summary>
    public GameObject CreateMesh(List<Vector3> vertices)
    {
        Initialize(vertices);

        while (true)
        {
            KeyValuePair<int, bool>[] left = _verticesBuffer.Where(buf => !buf.Value).ToArray();
            if (left.Length <= 3)
            {
                break;
            }
            DetecteTriangle();
        }

        int[] keys = _verticesBuffer.Keys.ToArray();
        foreach (int key in keys)
        {
            if (!_verticesBuffer[key])
            {
                _verticesBuffer[key] = true;
                _triangles.Add(key);
                Debug.LogFormat("_triangles.Add: {0}", key);
            }
        }

        Debug.Log("Done chekcing.");

        Mesh mesh = new Mesh();
        mesh.vertices = _vertices.ToArray();

        mesh.triangles = _triangles.ToArray();
        mesh.RecalculateNormals();

        GameObject go = new GameObject("MeshObject", typeof(MeshFilter), typeof(MeshRenderer));

        MeshFilter filter = go.GetComponent<MeshFilter>();
        filter.mesh = mesh;

        /*
        {
            GameObject dot = GameObject.CreatePrimitive(PrimitiveType.Sphere);
            dot.transform.localScale = Vector3.one * 0.5f;
            dot.transform.position = new Vector3(0, 0, 0);
            //dot.GetComponent<MeshRenderer>().material = mesh;
            Destroy(dot.GetComponent<Collider>());
        }

        if (PointInPolygon(new Vector3(0,0,0), _vertices))
        {
            Debug.Log("targetが内部に存在する.");
        }
        else
        {
            Debug.Log("targetが内部に存在しない.");
        }

        Vector3 lineStart = new Vector3(2, 0, 2);
        Vector3 lineEnd = new Vector3(4, 0, 4);
        Vector3 point = new Vector3(-2.1f, 0, -2.11f);

        Vector3 v, w;
        v = lineEnd - lineStart;
        w = point - lineStart;
        bool crossCheck = (0 == Vector3.Cross(v, w).sqrMagnitude);

        //bool check = LinePointCheck(lineStart, lineEnd, point);
        if (crossCheck)
        {
            Debug.Log("ベクトルは平行である");
        }
        else
        {
            Debug.Log("ベクトルは平行でない");
        }
        */


        return go;
    }

    /// <summary>
    /// Detect triangle from far point.
    /// </summary>
    private void DetecteTriangle()
    {
        if (!_isIncluding)
        {
            FindFarPoint();
        }

        Vector3 a = CurrentPoint;
        Vector3 b = NextPoint;
        Vector3 c = PreviousPoiont;

        Vector3 edge1 = b - a;
        Vector3 edge2 = c - a;

        float angle = Vector3.Angle(edge1, edge2);
        if (angle >= 180)
        {
            Debug.LogError("Something was wrong.");
            return;
        }

        if (IsIncludePoint())
        {
            Debug.Log("Point is including.");

            // try to find other point.
            _isIncluding = true;

            // Store current triangle dicretion.
            _prevDirection = GetCurrentDirection();

            MoveToNext();

            return;
        }

        _isIncluding = false;

        _triangles.Add(_curIndex);
        _triangles.Add(_nextIndex);
        _triangles.Add(_prevIndex);
        Debug.LogFormat("_triangles.Add(_curIndex): {0}", _curIndex);
        Debug.LogFormat("_triangles.Add(_nextIndex): {0}", _nextIndex);
        Debug.LogFormat("_triangles.Add(_prevIndex): {0}", _prevIndex);

        bool isDtected = true;
        _verticesBuffer[_curIndex] = isDtected;
    }

    /// <summary>
    /// Check to include point in the triangle.
    /// </summary>
    /// <returns></returns>
    private bool IsIncludePoint()
    {
        foreach (var key in _verticesBuffer.Keys)
        {
            int index = key;

            if (_verticesBuffer[key])
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

    /// <summary>
    /// Get current triangle direction.
    /// </summary>
    /// <returns>Triagnel direction normal.</returns>
    private Vector3 GetCurrentDirection()
    {
        Vector3 edge1 = (NextPoint - CurrentPoint).normalized;
        Vector3 edge2 = (PreviousPoiont - CurrentPoint).normalized;

        return Vector3.Cross(edge1, edge2);
    }

    /// <summary>
    /// Check including point.
    /// </summary>
    /// <param name="target">Target point.</param>
    /// <returns>return true if point is including.</returns>
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

    /// <summary>
    /// Poition reference move to next.
    /// </summary>
    private void MoveToNext()
    {
        _curIndex = FindNextIndex(_curIndex);
        _nextIndex = FindNextIndex(_curIndex);
        _prevIndex = FindPrevIndex(_curIndex);
    }

    /// <summary>
    /// 原点から最も遠い点を探す
    /// </summary>
    private void FindFarPoint()
    {
        int farIndex = -1;
        float maxDist = float.MinValue;

        foreach (var key in _verticesBuffer.Keys)
        {
            if (_verticesBuffer[key])
            {
                continue;
            }

            float dist = Vector3.Distance(Vector3.zero, _vertices[key]);
            if (dist > maxDist)
            {
                maxDist = dist;
                farIndex = key;
            }
        }

        _curIndex = farIndex;
        _nextIndex = FindNextIndex(_curIndex);
        _prevIndex = FindPrevIndex(_curIndex);
    }

    /// <summary>
    /// 指定インデックスから調べて次の有効頂点インデックスを探す
    /// </summary>
    private int FindNextIndex(int start)
    {
        int i = start;
        while (true)
        {
            i = (i + 1) % _vertices.Count;
            if (!_verticesBuffer[i])
            {
                return i;
            }
        }
    }

    /// <summary>
    /// 指定インデックスから調べて前の有効頂点インデックスを探す
    /// </summary>
    private int FindPrevIndex(int start)
    {
        int i = start;
        while (true)
        {
            i = (i - 1) >= 0 ? i - 1 : _vertices.Count - 1;
            if (!_verticesBuffer[i])
            {
                return i;
            }
        }
    }


    /*
    /// 
    /// 調査点が多角形の内外どちらにあるかを判定する
    /// 

    /// 多角形を構成する頂点リスト
    /// 調査点
    /// 多角形が存在する平面の法線
    /// 調査点が内側にある場合はtrue
    static public bool Check(Vector3[] points, Vector3 target, Vector3 normal)
    {
        // XY平面上に写像した状態で計算を行う
        Quaternion rot = Quaternion.FromToRotation(normal, -Vector3.forward);

        Vector3[] rotPoints = new Vector3[points.Length];

        for (int i = 0; i < rotPoints.Length; i++)
        {
            rotPoints[i] = rot * points[i];
        }

        target = rot * target;

        int wn = 0;
        float vt = 0;

        for (int i = 0; i < rotPoints.Length; i++)
        {
            // 上向きの辺、下向きの辺によって処理を分ける

            int cur = i;
            int next = (i + 1) % rotPoints.Length;

            // 上向きの辺。点PがY軸方向について、始点と終点の間にある。（ただし、終点は含まない）
            if ((rotPoints[cur].y <= target.y) && (rotPoints[next].y > target.y))
            {
                // 辺は点Pよりも右側にある。ただし重ならない
                // 辺が点Pと同じ高さになる位置を特定し、その時のXの値と点PのXの値を比較する
                vt = (target.y - rotPoints[cur].y) / (rotPoints[next].y - rotPoints[cur].y);

                if (target.x < (rotPoints[cur].x + (vt * (rotPoints[next].x - rotPoints[cur].x))))
                {
                    // 上向きの辺と交差した場合は+1
                    wn++;
                }
            }
            else if ((rotPoints[cur].y > target.y) && (rotPoints[next].y <= target.y))
            {
                // 辺は点Pよりも右側にある。ただし重ならない
                // 辺が点Pと同じ高さになる位置を特定し、その時のXの値と点PのXの値を比較する
                vt = (target.y - rotPoints[cur].y) / (rotPoints[next].y - rotPoints[cur].y);

                if (target.x < (rotPoints[cur].x + (vt * (rotPoints[next].x - rotPoints[cur].x))))
                {
                    // 下向きの辺と交差した場合は-1
                    wn--;
                }
            }
        }

        return wn != 0;
    }
    */

    public static bool PointInPolygon(Vector3 testPoint, List<Vector3> vertices)
    {
        // Sanity check - not enough bounds vertices = nothing to be inside of
        if (vertices.Count < 3)
            return false;

        // Check how many lines this test point collides with going in one direction
        // Odd = Inside, Even = Outside
        var collisions = 0;
        var vertexCounter = 0;
        var startPoint = vertices[vertices.Count - 1];

        // We recenter the test point around the origin to simplify the math a bit
        startPoint.x -= testPoint.x;
        startPoint.z -= testPoint.z;

        var currentSide = false;
        if (!ApproximatelyZero(startPoint.z))
        {
            currentSide = startPoint.z < 0f;
        }
        else
        {
            // We need a definitive side of the horizontal axis to start with (since we need to know when we
            // cross it), so we go backwards through the vertices until we find one that does not lie on the horizontal
            for (var i = vertices.Count - 2; i >= 0; --i)
            {
                var vertZ = vertices[i].z;
                vertZ -= testPoint.z;
                if (!ApproximatelyZero(vertZ))
                {
                    currentSide = vertZ < 0f;
                    break;
                }
            }
        }

        while (vertexCounter < vertices.Count)
        {
            var endPoint = vertices[vertexCounter];
            endPoint.x -= testPoint.x;
            endPoint.z -= testPoint.z;

            var startToEnd = endPoint - startPoint;
            var edgeSqrMagnitude = startToEnd.sqrMagnitude;
            if (ApproximatelyZero(startToEnd.x * endPoint.z - startToEnd.z * endPoint.x) &&
                startPoint.sqrMagnitude <= edgeSqrMagnitude && endPoint.sqrMagnitude <= edgeSqrMagnitude)
            {
                // This line goes through the start point, which means the point is on an edge of the polygon
                return true;
            }

            // Ignore lines that end at the horizontal axis
            if (!ApproximatelyZero(endPoint.z))
            {
                var nextSide = endPoint.z < 0f;
                if (nextSide != currentSide)
                {
                    currentSide = nextSide;

                    // If we've crossed the horizontal, check if the origin is to the left of the line
                    if ((startPoint.x * endPoint.z - startPoint.z * endPoint.x) / -(startPoint.z - endPoint.z) > 0)
                        collisions++;
                }
            }

            startPoint = endPoint;
            vertexCounter++;
        }

        return collisions % 2 > 0;
    }

    internal static readonly float EpsilonScaled = Mathf.Epsilon * 8;
    public static bool ApproximatelyZero(float a)
    {
        return (a >= 0f ? a : -a) < EpsilonScaled;
    }


    public bool LinePointCheck(Vector3 lineStart, Vector3 lineEnd, Vector3 point)
    {
        Vector3 v, w;
        float vl, wl;
        v = lineEnd - lineStart;
        w = point - lineStart;

        vl = v.sqrMagnitude;
        wl = w.sqrMagnitude;

        Debug.LogFormat("Vector3.Cross(v, w).sqrMagnitude): {0}", Vector3.Cross(v, w).sqrMagnitude);
        Debug.LogFormat("Vector3.Dot(v, w): {0}", Vector3.Dot(v, w));

        bool crossCheck = (0 == Vector3.Cross(v, w).sqrMagnitude);
        bool dotCheck = (0 <= Vector3.Dot(v, w));
        return crossCheck && dotCheck && vl >= wl;
    }

}