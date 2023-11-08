using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System.Text;
using System;
using System.Reflection;

using ClipperLib;
using Clipper2Lib;
namespace ClipperTest1
{
    using ClipperLib_Path = List<IntPoint>;
    using ClipperLib_Paths = List<List<IntPoint>>;



    public class PointDrawer : MonoBehaviour
    {
        [SerializeField]
        private DrawMesh _drawMesh;

        [SerializeField]
        private Material _dotMat;

        [SerializeField]
        private float _dotSize = 0.05f;

        [SerializeField]
        private Material _material;

        [SerializeField]
        private Material _material_test;

        [SerializeField]
        private float _threshold = 0.1f;

        private float _sqrThreshold = 0;

        private List<Vector3> _samplingVertices = new List<Vector3>();

        private List<GameObject> _dotList = new List<GameObject>();
        //private List<Vector3> _vertices = new List<Vector3>();
        private List<ElementVector2> _vertices = new List<ElementVector2>();
        private List<GameObject> _meshList = new List<GameObject>();

        private Vector3 prevPosition = new Vector3(0, 0, 0);

        /// <summary>
        /// Get average point.
        /// </summary>
        private Vector3 AveragePoint
        {
            get
            {
                Vector3 avg = Vector3.zero;
                for (int i = 0; i < _samplingVertices.Count; i++)
                {
                    avg += _samplingVertices[i];
                }
                avg /= _samplingVertices.Count;

                return avg;
            }
        }

        GameObject gLineObject;
        LineRenderer gLineRenderer;

        public class ElementVector2
        {
            public Vector2 point;
            public int intersect_index;
            public int clockwise;

            public ElementVector2(Vector2 _point, int _intersect_index = -1, int _clockwise = -1)
            {
                point = _point;
                intersect_index = _intersect_index;
                clockwise = _clockwise;
            }
        }

        public int g_intersect_index = 0;


        private void Awake()
        {
            _sqrThreshold = _threshold * _threshold;
        }

        void Start()
        {
            //Debug.Log(LineIntersect(new Vector2(1, 1), new Vector2(4, 3), new Vector2(3, 1), new Vector2(1, 3)));

            gLineObject = new GameObject();
            gLineObject.transform.position = new Vector3(0, 0, -0.2f);
            gLineRenderer = gLineObject.AddComponent<LineRenderer>();
            gLineRenderer.useWorldSpace = false;
            gLineRenderer.startWidth = 0.1f;
            gLineRenderer.endWidth = 0.1f;

            /*
            List<Vector3> v = new List<Vector3>();

            //Paths64 simpleShapes = DoSimpleShapes();
            //PathsD simpleShapes = DoVariableOffset();
            PathsD simpleShapes = DoUnion();

            for (int i = 0; i < simpleShapes.Count; i++)
            {
                GameObject lineObject = new GameObject();
                lineObject.transform.position = new Vector3(0, 0, -0.2f);
                LineRenderer lineRenderer = lineObject.AddComponent<LineRenderer>();
                lineRenderer.useWorldSpace = false;
                lineRenderer.startWidth = 0.1f;
                lineRenderer.endWidth = 0.1f;

                for (int k = 0; k < simpleShapes[i].Count; k++)
                {
                    v.Add(new Vector3((float)simpleShapes[i][k].x * 0.1f, (float)simpleShapes[i][k].y * 0.1f, 0));
                    CreateDot(v[v.Count - 1]);
                }
                lineRenderer.positionCount = v.Count;
                lineRenderer.SetPositions(v.ToArray());
            }
            */
        }

        private void Update()
        {
            if (Input.GetMouseButton(0))
            {
                TryRaycast();
            }

            if (Input.GetMouseButtonUp(0))
            {
                /*
                GameObject go = _drawMesh.CreateMesh(_vertices);
                go.GetComponent<MeshRenderer>().material = _material;
                go.transform.position += go.transform.forward * -0.001f;
                _meshList.Add(go);
                */

                /*
                _vertices.Add(_vertices[0]);


                List<Vector3> v = testFixVector(_vertices);


                GameObject mGameObject = new GameObject();

                // LineRendererコンポーネントをゲームオブジェクトにアタッチする
                var lineRenderer = mGameObject.AddComponent<LineRenderer>();
                lineRenderer.useWorldSpace = false;
                // 点の数を指定する
                lineRenderer.positionCount = v.Count;
                // 線を引く場所を指定する
                lineRenderer.SetPositions(v.ToArray());
                lineRenderer.startWidth = 0.1f;                   // 開始点の太さを0.1にする
                lineRenderer.endWidth = 0.1f;
                mGameObject.transform.position = new Vector3(0, 0, -0.5f);
                */

                /*
                bool b_ = PointInPolygon(_vertices[_vertices.Count - 1].point, _vertices);
                if (b_)
                {
                    Debug.Log("targetが内部に存在する.");
                }
                else
                {
                    Debug.Log("targetが内部に存在しない.");
                }
                */



                //ClearDot();
                //testFixVector();
                /*
                List<Vector2> v = new List<Vector2>();
                for (int i = 0; i < _vertices.Count; i++)
                {
                    v.Add(new Vector2(_vertices[i].point.x, _vertices[i].point.y));
                }
                List<Vector2> cleanedPolygon = GetConvexHull(v);
                List<Vector3> v3 = new List<Vector3>();
                for (int i = 0; i < cleanedPolygon.Count; i++)
                {
                    v3.Add(new Vector3(cleanedPolygon[i].x, cleanedPolygon[i].y, 0));
                    //CreateDot(v3[i]);
                }
                // 点の数を指定する
                gLineRenderer.positionCount = v3.Count;
                // 線を引く場所を指定する
                gLineRenderer.SetPositions(v3.ToArray());
                gLineRenderer.loop = true;
                */

                List<Vector2> v = new List<Vector2>();
                for (int i = 0; i < _vertices.Count; i++)
                {
                    v.Add(new Vector2(_vertices[i].point.x, _vertices[i].point.y));
                }
                bool b_ = IsPointInsidePolygon(v[v.Count - 1], v);
                if (b_)
                {
                    Debug.Log("targetが内部に存在する.");
                }
                else
                {
                    Debug.Log("targetが内部に存在しない.");
                }

                //Debug.Log("gLineRenderer: " + v.Count);
                //_vertices.Clear();
                //_vertices = v;
                //for (int i = 0; i < _vertices.Count; i++)
                //{
                //    //_vertices.Add(new Vector3(v[i].x, v[i].y, v[i].z));
                //    
                //}

                //Path path = new Path();
                /*
                System.Random r = new System.Random();
                Path path = MakeRandomPolygon(r, 10, 10, 6);
                Paths paths = ClipperLib.Clipper.SimplifyPolygon(path, PolyFillType.pftPositive);
                Debug.Log("targetが内部に存在しない.");
                */

                //SimplifyPath(PathD path, double epsilon, bool isOpenPath = false)
            }

            if (Input.GetKeyDown(KeyCode.Q))
            {
                Clear();
            }
            if (Input.GetKeyDown(KeyCode.A))
            {
                //DrawOrigin();
                //DrawBoundary();
                //test2();


                ClearDot();
                List<Vector2> v2 = new List<Vector2>();
                for (int i = 0; i < _vertices.Count; i++)
                {
                    v2.Add(new Vector2(_vertices[i].point.x, _vertices[i].point.y));
                }
                //PathsD simpleShapes = Union(v2);
                ClipperLib_Paths simpleShapes = SimplifyPolygon(v2);

                for (int i = 0; i < simpleShapes.Count; i++)
                {
                    GameObject lineObject = new GameObject();
                    lineObject.transform.position = new Vector3(0, 0, -0.2f);
                    LineRenderer lineRenderer = lineObject.AddComponent<LineRenderer>();
                    lineRenderer.useWorldSpace = false;
                    lineRenderer.startWidth = 0.1f;
                    lineRenderer.endWidth = 0.1f;

                    List<Vector3> v = new List<Vector3>();
                    for (int k = 0; k < simpleShapes[i].Count; k++)
                    {
                        //PathD pathList = simpleShapes[i];
                        ClipperLib_Path pathList = simpleShapes[i];
                        //v.Add(new Vector3((float)pathList[k].x, (float)pathList[k].y, 0));
                        v.Add(new Vector3((float)pathList[k].X / 100.0f, (float)pathList[k].Y / 100.0f, 0));
                        CreateDot(v[v.Count - 1]);
                    }
                    lineRenderer.positionCount = v.Count;
                    lineRenderer.SetPositions(v.ToArray());
                    lineRenderer.loop = true;
                }
            }
        }


        /// <summary>
        /// Try raycast to the plane.
        /// </summary>
        private void TryRaycast()
        {
            Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
            RaycastHit hit;
            if (Physics.Raycast(ray, out hit, float.MaxValue))
            {
                if (_vertices.Count == 0)
                {
                    AddVertex(hit.point);
                    return;
                }

                if (_samplingVertices.Count == 0)
                {
                    _samplingVertices.Add(hit.point);
                    return;
                }

                float dist = (AveragePoint - hit.point).sqrMagnitude;
                if (dist >= _sqrThreshold)
                {
                    AddVertex(hit.point);
                    _samplingVertices.Clear();
                    /*
                    testFixVector2(new Vector2(hit.point.x, hit.point.y));
                    //vertices.Add(vertices[0]);
                    ClearDot();
                    //testFixVector();
                    List<Vector3> v = new List<Vector3>();
                    for (int i = 0; i < _vertices.Count; i++)
                    {
                        v.Add(new Vector3(_vertices[i].point.x, _vertices[i].point.y, 0));
                        CreateDot(v[i]);
                    }
                    */
                    // 点の数を指定する
                    //gLineRenderer.positionCount = v.Count;
                    // 線を引く場所を指定する
                    //gLineRenderer.SetPositions(v.ToArray());
                    

                    //Debug.Log("gLineRenderer: " + v.Count);
                    //_vertices.Clear();
                    //_vertices = v;
                    //for (int i = 0; i < _vertices.Count; i++)
                    //{
                    //    //_vertices.Add(new Vector3(v[i].x, v[i].y, v[i].z));
                    //    
                    //}
                }
                else
                {
                    _samplingVertices.Add(hit.point);
                }
            }
        }

        private void AddVertex(Vector3 point)
        {
            CreateDot(point);
            //DrawTest(point);
            _vertices.Add(new ElementVector2(point, -1));
            _samplingVertices.Clear();
        }

        /// <summary>
        /// Create dot for clicked poisition.
        /// </summary>
        /// <returns>Dot GameObject.</returns>
        private GameObject CreateDot(Vector3 position)
        {
            //Debug.Log("Create dot.");

            GameObject dot = GameObject.CreatePrimitive(PrimitiveType.Sphere);
            dot.transform.localScale = Vector3.one * _dotSize;
            dot.transform.position = position;
            dot.GetComponent<MeshRenderer>().material = _dotMat;
            Destroy(dot.GetComponent<Collider>());

            Vector3 eVec = position - prevPosition;
            //Debug.Log(Mathf.Atan2(eVec.y, eVec.x));
            prevPosition = position;

            _dotList.Add(dot);

            return dot;
        }

        public void Clear()
        {
            for (int i = 0; i < _dotList.Count; i++)
            {
                Destroy(_dotList[i]);
            }
            _dotList.Clear();

            for (int i = 0; i < _meshList.Count; i++)
            {
                Destroy(_meshList[i]);
            }
            _meshList.Clear();
        }

        public void ClearDot()
        {
            for (int i = 0; i < _dotList.Count; i++)
            {
                Destroy(_dotList[i]);
            }
            _dotList.Clear();
        }

        /*
        public void DrawBoundary()
        {

            float angle = 90;
            float rad = angle * Mathf.Deg2Rad;
            Vector3 vec = new Vector3(Mathf.Cos(rad), Mathf.Sin(rad), 0);

            GameObject dot = GameObject.CreatePrimitive(PrimitiveType.Sphere);
            dot.transform.localScale = Vector3.one * _dotSize;
            dot.transform.position = position;
            dot.GetComponent<MeshRenderer>().material = _dotMat;
            Destroy(dot.GetComponent<Collider>());
        }
        */
        public void DrawTest(Vector3 position)
        {

            float angle = 90;
            float rad = angle * Mathf.Deg2Rad;
            /*
            Vector3 vec = new Vector3(Mathf.Cos(rad) * position.x, Mathf.Sin(rad) * position.y, position.z);
            */

            Vector3 vec = Quaternion.Euler(0, 0, angle) * position;

            GameObject dot = GameObject.CreatePrimitive(PrimitiveType.Sphere);
            dot.transform.localScale = Vector3.one * _dotSize;
            dot.transform.position = vec;
            dot.GetComponent<MeshRenderer>().material = _dotMat;
            Destroy(dot.GetComponent<Collider>());


            //原点を中心としたベクトルに対して垂直な頂点
            //float angle = 180;
            //Vector3 vec = Quaternion.Euler(0, 0, angle) * position;
        }

        public void DrawOrigin()
        {
            GameObject dot = GameObject.CreatePrimitive(PrimitiveType.Sphere);
            dot.transform.localScale = Vector3.one * _dotSize;
            dot.transform.position = new Vector3(0, 0, 0);
            dot.GetComponent<MeshRenderer>().material = _material_test;
            Destroy(dot.GetComponent<Collider>());
        }

        Vector2 LineIntersect(Vector2 origin1, Vector2 direction1, Vector2 origin2, Vector2 direction2)
        {
            Vector2 intersect = Vector2.zero;

            Vector2 slopeV1 = origin1 - direction1;
            float slopeF1 = slopeV1.y / slopeV1.x;

            Vector2 slopeV2 = origin2 - direction2;
            float slopeF2 = slopeV2.y / slopeV2.x;

            intersect.x = (slopeF1 * origin1.x - slopeF2 * origin2.x + origin2.y - origin1.y) / (slopeF1 - slopeF2);
            intersect.y = slopeF1 * (intersect.x - origin1.x) + origin1.y;

            return intersect;
        }

        void test2()
        {
            GameObject mGameObject = new GameObject();
            // LineRendererコンポーネントをゲームオブジェクトにアタッチする
            var lineRenderer = mGameObject.AddComponent<LineRenderer>();

            /*
            var positions = new Vector3[]{
                new Vector3(0, 0, 0),               // 開始点
                new Vector3(8, 0, 0),               // 中継点
                new Vector3(8, -3, 0),              // 終了点
                new Vector3(0, 0, 0),              // 終了点
            };
            */

            int circleVertexNum = _vertices.Count;
            var positions = new Vector3[circleVertexNum + 1];

            for (int i = 0; i < circleVertexNum; i++)
            {
                //positions[i] = _vertices[i];
            }
            positions[circleVertexNum] = positions[0];

            // 点の数を指定する
            lineRenderer.positionCount = positions.Length;

            // 線を引く場所を指定する
            lineRenderer.SetPositions(positions);

            //lineRenderer.numCapVertices = 4;
            //lineRenderer.numCornerVertices = 4;
            //lineRenderer.loop = true;
            //lineRenderer.widthMultiplier = 0.1f;

            //====================================================
            int vertexCount = lineRenderer.positionCount;
            Vector3[] linePositions = new Vector3[vertexCount];
            lineRenderer.GetPositions(linePositions);

            // Create a new mesh
            Mesh mesh = new Mesh();
            mesh.name = "Generated Mesh";

            // Define vertices
            mesh.vertices = linePositions;

            // Create triangles to form the mesh
            int[] triangles = new int[(vertexCount - 2) * 3];
            for (int i = 0; i < vertexCount - 2; i++)
            {
                triangles[i * 3] = 0;
                triangles[i * 3 + 1] = i + 1;
                triangles[i * 3 + 2] = i + 2;
            }

            // Assign vertices and triangles to the mesh
            mesh.triangles = triangles;

            // Optionally, you can recalculate normals and optimize the mesh
            mesh.RecalculateNormals();
            mesh.Optimize();

            // Create a new GameObject to display the mesh
            GameObject meshObject = new GameObject("Generated Mesh Object");
            MeshFilter meshFilter = meshObject.AddComponent<MeshFilter>();
            meshFilter.mesh = mesh;

            MeshRenderer meshRenderer = meshObject.AddComponent<MeshRenderer>();
            // Set material, shader, and texture if desired
        }



        ///////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////


        static Vector2 FindIntersection(Vector2 p1, Vector2 q1, Vector2 p2, Vector2 q2)
        {
            float A1 = q1.y - p1.y;
            float B1 = p1.x - q1.x;
            float C1 = A1 * p1.x + B1 * p1.y;

            float A2 = q2.y - p2.y;
            float B2 = p2.x - q2.x;
            float C2 = A2 * p2.x + B2 * p2.y;

            float det = A1 * B2 - A2 * B1;

            //平行な場合を考慮しない
            float x = (B2 * C1 - B1 * C2) / det;
            float y = (A1 * C2 - A2 * C1) / det;
            return new Vector2(x, y);
        }

        static bool DoIntersect(Vector2 v0_start, Vector2 v0_end, Vector2 v1_start, Vector2 v1_end)
        {
            Vector2 p1 = v0_start;
            Vector2 q1 = v0_end;
            Vector2 p2 = v1_start;
            Vector2 q2 = v1_end;

            float cross1 = (q1.x - p1.x) * (p2.y - p1.y) - (q1.y - p1.y) * (p2.x - p1.x);
            float cross2 = (q1.x - p1.x) * (q2.y - p1.y) - (q1.y - p1.y) * (q2.x - p1.x);
            float cross3 = (q2.x - p2.x) * (p1.y - p2.y) - (q2.y - p2.y) * (p1.x - p2.x);
            float cross4 = (q2.x - p2.x) * (q1.y - p2.y) - (q2.y - p2.y) * (q1.x - p2.x);

            return cross1 * cross2 < 0 && cross3 * cross4 < 0;
        }

        void testFixVector()
        {
            List<Vector2> vertices = new List<Vector2>();
            for (int i = 0; i < _vertices.Count; i++)
            {
                vertices.Add(new Vector2(_vertices[i].point.x, _vertices[i].point.y));
            }
            //Debug.Log("vertices.Count : " + vertices.Count);

            List<ElementVector2> result_vertices = new List<ElementVector2>();

            Vector2 main_cur_vertex = Vector2.zero;
            Vector2 main_prev_vertex = Vector2.zero;
            Vector2 sub_cur_vertex = Vector2.zero;
            Vector2 sub_prev_vertex = Vector2.zero;
            Vector2 sub2_cur_vertex = Vector2.zero;
            Vector2 sub2_prev_vertex = Vector2.zero;
            bool is_first = true;
            bool is_boundary = false;
            result_vertices.Add(new ElementVector2(vertices[0]));
            //foreach (var main_vertex in vertices)
            for (int a = 0; a < vertices.Count; a++)
            {
                //Debug.Log("vertices : " + main_vertex);
                if (is_boundary && a == (vertices.Count - 1))
                {
                    //Debug.Log("break: " + a);
                    //break;
                }
                var main_vertex = vertices[a];
                main_prev_vertex = main_cur_vertex;
                main_cur_vertex = main_vertex;
                if (is_first)
                {
                    is_first = false;
                    continue;
                }

                for (int i = 0; i < result_vertices.Count; i++)
                {
                    //Debug.Log("result_vertices : " + i);
                    sub_prev_vertex = sub_cur_vertex;
                    sub_cur_vertex = result_vertices[i].point;
                    if (i == 0) { continue; }
                    if (DoIntersect(main_prev_vertex, main_cur_vertex, sub_prev_vertex, sub_cur_vertex))
                    {
                        is_boundary = true;
                        Vector2 intersection = FindIntersection(main_prev_vertex, main_cur_vertex, sub_prev_vertex, sub_cur_vertex);

                        ElementVector2 subVertex = new ElementVector2(intersection, 0);
                        ElementVector2 mainVertex = new ElementVector2(intersection, 0);

                        //時計回りか計算
                        List<ElementVector2> clockwise_vertices = new List<ElementVector2>();
                        clockwise_vertices.Add(subVertex);
                        for (int k = i; k < result_vertices.Count; k++)
                        {
                            clockwise_vertices.Add(result_vertices[k]);
                        }
                        //内側
                        for (int k = 0; k < clockwise_vertices.Count; k++)
                        {
                            if (k == 0) { continue; }
                            if (clockwise_vertices[k].intersect_index >= 0)
                            {
                                while (true)
                                {
                                    clockwise_vertices.RemoveAt(k);
                                    if (clockwise_vertices.Count <= k)
                                    {
                                        Debug.LogError("Array Error: " + i + ", Len: " + clockwise_vertices.Count);
                                    }
                                    if (clockwise_vertices[k].intersect_index >= 0)
                                    {
                                        break;
                                    }
                                }
                            }
                        }
                        List<Vector2> clockwise_vertices_vec = new List<Vector2>();
                        for (int k = 0; k < clockwise_vertices.Count; k++)
                        {
                            clockwise_vertices_vec.Add(clockwise_vertices[k].point);
                        }
                        int is_clockwise = 0;
                        if (IsClockwise(clockwise_vertices_vec.ToArray()))
                        {
                            //Debug.Log("時計回りです: " + clockwise_vertices_vec[0]);
                            is_clockwise = 0;
                        }
                        else
                        {
                            //Debug.Log("反時計回りです: " + clockwise_vertices_vec[0]);
                            subVertex.intersect_index = 1;
                            mainVertex.intersect_index = 1;
                            is_clockwise = 1;
                        }

                        //sub
                        result_vertices.Insert(i, subVertex);

                        //逆回転
                        List<ElementVector2> reverse_vertices = new List<ElementVector2>();
                        while (true)
                        {
                            if (i == result_vertices.Count - 1)
                            {
                                break;
                            }
                            int index = result_vertices.Count - 1;
                            reverse_vertices.Add(new ElementVector2(result_vertices[index].point, result_vertices[index].intersect_index));
                            result_vertices.RemoveAt(index);
                        }
                        for (int k = 0; k < reverse_vertices.Count; k++)
                        {
                            result_vertices.Add(new ElementVector2(reverse_vertices[k].point, reverse_vertices[k].intersect_index));
                        }

                        //内側の領域削除処理
                        for (int k = i + 1; k < result_vertices.Count; k++)
                        {
                            if (result_vertices[k].intersect_index == is_clockwise)
                            {
                                while (true)
                                {
                                    //Debug.Log("loop");
                                    result_vertices.RemoveAt(k);
                                    if (result_vertices[k].intersect_index >= 0)
                                    {
                                        break;
                                    }
                                }
                                result_vertices[k].intersect_index = -1;
                            }
                        }
                        //main
                        result_vertices.Add(mainVertex);

                        /*
                        //sub
                        result_vertices.Insert(i, subVertex);
                        //main
                        result_vertices.Add(mainVertex);
                        */

                        break;
                    }
                }

                result_vertices.Add(new ElementVector2(main_cur_vertex));
            }

            //List<Vector3> return_vertices = new List<Vector3>();
            _vertices.Clear();
            for (int i = 0; i < result_vertices.Count; i++)
            {
                //return_vertices.Add(new Vector3(result_vertices[i].point.x, result_vertices[i].point.y, 0));
                _vertices.Add(new ElementVector2(result_vertices[i].point, result_vertices[i].intersect_index));

            }

            //return return_vertices;
        }

        public bool PointInPolygon(Vector2 testPoint, List<ElementVector2> vertices)
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
            startPoint.point.x -= testPoint.x;
            startPoint.point.y -= testPoint.y;

            var currentSide = false;
            if (!ApproximatelyZero(startPoint.point.y))
            {
                currentSide = startPoint.point.y < 0f;
            }
            else
            {
                // We need a definitive side of the horizontal axis to start with (since we need to know when we
                // cross it), so we go backwards through the vertices until we find one that does not lie on the horizontal
                for (var i = vertices.Count - 2; i >= 0; --i)
                {
                    var vertZ = vertices[i].point.y;
                    vertZ -= testPoint.y;
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
                endPoint.point.x -= testPoint.x;
                endPoint.point.y -= testPoint.y;

                var startToEnd = endPoint.point - startPoint.point;
                var edgeSqrMagnitude = startToEnd.sqrMagnitude;
                if (ApproximatelyZero(startToEnd.x * endPoint.point.y - startToEnd.y * endPoint.point.x) &&
                    startPoint.point.sqrMagnitude <= edgeSqrMagnitude && endPoint.point.sqrMagnitude <= edgeSqrMagnitude)
                {
                    // This line goes through the start point, which means the point is on an edge of the polygon
                    return true;
                }

                // Ignore lines that end at the horizontal axis
                if (!ApproximatelyZero(endPoint.point.y))
                {
                    var nextSide = endPoint.point.y < 0f;
                    if (nextSide != currentSide)
                    {
                        currentSide = nextSide;

                        // If we've crossed the horizontal, check if the origin is to the left of the line
                        if ((startPoint.point.x * endPoint.point.y - startPoint.point.y * endPoint.point.x) / -(startPoint.point.y - endPoint.point.y) > 0)
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

        bool IsClockwise(Vector2[] polygon)
        {
            int n = polygon.Length;
            if (n < 3)
            {
                // 多角形が3つ未満の頂点を持つ場合、判定ができません
                throw new ArgumentException("多角形は少なくとも3つの頂点を持つ必要があります");
            }

            double sum = 0;

            for (int i = 0; i < n; i++)
            {
                int next = (i + 1) % n;
                sum += (polygon[next].x - polygon[i].x) * (polygon[next].y + polygon[i].y);
            }

            return sum > 0;
        }

        double cross(Vector2 cur, Vector2 next)
        {
            return (next.x - cur.x) * (next.y + cur.y);
        }

        //新
        void testFixVector2(Vector2 _addVertex)
        {

            //List<ElementVector2> result_vertices = new List<ElementVector2>();

            Vector2 _addVertexPrev = _vertices[_vertices.Count - 1].point;

            Vector2 main_cur_vertex = Vector2.zero;
            Vector2 main_prev_vertex = Vector2.zero;
            bool is_boundary = false;
            for (int i = 0; i < _vertices.Count; i++)
            {
                /*
                if (is_boundary && i == (_vertices.Count - 1))
                {
                    //Debug.Log("break: " + a);
                    //break;
                }
                */
                var main_vertex = _vertices[i];
                main_prev_vertex = main_cur_vertex;
                main_cur_vertex = main_vertex.point;
                if (i == 0)
                {
                    continue;
                }

                if (DoIntersect(_addVertexPrev, _addVertex, main_prev_vertex, main_cur_vertex))
                {
                    is_boundary = true;
                    Vector2 intersection = FindIntersection(_addVertexPrev, _addVertex, main_prev_vertex, main_cur_vertex);

                    ElementVector2 subVertex = new ElementVector2(intersection, g_intersect_index);
                    ElementVector2 mainVertex = new ElementVector2(intersection, g_intersect_index);
                    g_intersect_index++;

                    //時計回りか計算
                    List<ElementVector2> clockwise_vertices = new List<ElementVector2>();
                    clockwise_vertices.Add(subVertex);
                    for (int k = i; k < _vertices.Count; k++)
                    {
                        clockwise_vertices.Add(_vertices[k]);
                    }
                    /*
                    //内側
                    for (int k = 0; k < clockwise_vertices.Count; k++)
                    {
                        if (k == 0) { continue; }
                        if (clockwise_vertices[k].clockwise >= 0)
                        {
                            while (true)
                            {
                                clockwise_vertices.RemoveAt(k);
                                if (clockwise_vertices.Count <= k)
                                {
                                    Debug.LogError("Array Error: " + i + ", Len: " + clockwise_vertices.Count);
                                }
                                if (clockwise_vertices[k].intersect_index >= 0)
                                {
                                    break;
                                }
                            }
                        }
                    }
                    */
                    List<Vector2> clockwise_vertices_vec = new List<Vector2>();
                    for (int k = 0; k < clockwise_vertices.Count; k++)
                    {
                        clockwise_vertices_vec.Add(clockwise_vertices[k].point);
                    }
                    int is_clockwise = 0;
                    /*
                    if (IsClockwise(clockwise_vertices_vec.ToArray()))
                    {
                        //Debug.Log("時計回りです: " + clockwise_vertices_vec[0]);
                        is_clockwise = 0;
                    }
                    else
                    {
                        //Debug.Log("反時計回りです: " + clockwise_vertices_vec[0]);
                        subVertex.clockwise = 1;
                        mainVertex.clockwise = 1;
                        is_clockwise = 1;
                    }
                    */

                    //sub
                    _vertices.Insert(i, subVertex);

                    //逆回転
                    List<ElementVector2> reverse_vertices = new List<ElementVector2>();
                    while (true)
                    {
                        if (i == _vertices.Count - 1)
                        {
                            break;
                        }
                        int index = _vertices.Count - 1;
                        reverse_vertices.Add(new ElementVector2(_vertices[index].point, _vertices[index].intersect_index, _vertices[index].clockwise));
                        _vertices.RemoveAt(index);
                    }
                    for (int k = 0; k < reverse_vertices.Count; k++)
                    {
                        _vertices.Add(new ElementVector2(reverse_vertices[k].point, reverse_vertices[k].intersect_index, reverse_vertices[k].clockwise));
                    }
                    /*
                    //内側の領域削除処理
                    for (int k = i + 1; k < _vertices.Count; k++)
                    {
                        if (_vertices[k].clockwise == is_clockwise)
                        {
                            while (true)
                            {
                                //Debug.Log("loop");
                                _vertices.RemoveAt(k);
                                if (_vertices[k].clockwise >= 0)
                                {
                                    break;
                                }
                            }
                            _vertices[k].clockwise = -1;
                        }
                    }
                    */
                    //main
                    _vertices.Add(mainVertex);

                    /*
                    //sub
                    result_vertices.Insert(i, subVertex);
                    //main
                    result_vertices.Add(mainVertex);
                    */

                    break;
                }

                //_vertices.Add(new ElementVector2(main_cur_vertex));
            }
            _vertices.Add(new ElementVector2(_addVertex));

            /*
            //List<Vector3> return_vertices = new List<Vector3>();
            _vertices.Clear();
            for (int i = 0; i < result_vertices.Count; i++)
            {
                //return_vertices.Add(new Vector3(result_vertices[i].point.x, result_vertices[i].point.y, 0));
                _vertices.Add(new ElementVector2(result_vertices[i].point, result_vertices[i].intersect_index));

            }
            */

            //return return_vertices;
        }

        //=============================================================
        public static List<Vector2> GetConvexHull(List<Vector2> vertices)
        {
            if (vertices.Count < 3)
            {
                // Convex hull cannot be formed with less than 3 vertices.
                throw new ArgumentException("At least 3 vertices are required.");
            }

            // Sort vertices by X-coordinate (and then by Y-coordinate if X is equal).
            vertices.Sort((a, b) =>
            {
                int cmp = a.x.CompareTo(b.x);
                if (cmp == 0)
                {
                    return a.y.CompareTo(b.y);
                }
                return cmp;
            });

            // Initialize the upper and lower hulls.
            List<Vector2> upperHull = new List<Vector2>();
            List<Vector2> lowerHull = new List<Vector2>();

            // Build the upper hull.
            foreach (var vertex in vertices)
            {
                while (upperHull.Count >= 2 && IsCounterClockwise(upperHull[upperHull.Count - 2], upperHull[upperHull.Count - 1], vertex))
                {
                    upperHull.RemoveAt(upperHull.Count - 1);
                }
                upperHull.Add(vertex);
            }

            // Build the lower hull.
            for (int i = vertices.Count - 1; i >= 0; i--)
            {
                var vertex = vertices[i];
                while (lowerHull.Count >= 2 && IsCounterClockwise(lowerHull[lowerHull.Count - 2], lowerHull[lowerHull.Count - 1], vertex))
                {
                    lowerHull.RemoveAt(lowerHull.Count - 1);
                }
                lowerHull.Add(vertex);
            }

            // Remove the last vertex from each hull to avoid duplication.
            upperHull.RemoveAt(upperHull.Count - 1);
            lowerHull.RemoveAt(lowerHull.Count - 1);

            // Combine the upper and lower hulls to form the convex hull.
            upperHull.AddRange(lowerHull);
            return upperHull;
        }

        // Check if the points A, B, and C are in counter-clockwise order.
        private static bool IsCounterClockwise(Vector2 a, Vector2 b, Vector2 c)
        {
            return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x) > 0;
        }

        // 指定の点がポリゴン内に存在するかどうかを判定する関数
        static bool IsPointInsidePolygon(Vector2 point, List<Vector2> vertices)
        {
            int n = vertices.Count;
            bool isInside = false;

            for (int i = 0, j = n - 1; i < n; j = i++)
            {
                if ((vertices[i].y > point.y) != (vertices[j].y > point.y) &&
                    point.x < (vertices[j].x - vertices[i].x) * (point.y - vertices[i].y) / (vertices[j].y - vertices[i].y) + vertices[i].x)
                {
                    isInside = !isInside;
                }
            }

            return isInside;
        }
        // ===========================================================================
        // ===========================================================================
#if false
        public static void Main()
        {
            DoSimpleShapes();
            DoRabbit();
            DoVariableOffset();
        }

        public static Paths64 DoSimpleShapes()
        {
            //triangle offset - with large miter
            Paths64 p = new() { Clipper.MakePath(new int[] { 30, 150, 60, 350, 0, 350 }) };
            Paths64 pp = new();
            pp.AddRange(p);

            for (int i = 0; i < 5; ++i)
            {
                //nb: the last parameter here (10) greatly increases miter limit
                p = Clipper.InflatePaths(p, 5, JoinType.Miter, EndType.Polygon, 10);
                pp.AddRange(p);
            }

            //rectangle offset - both squared and rounded
            p.Clear();
            ClipperOffset co = new();

            pp.Clear();

            //nb: using the ClipperOffest class directly here to control 
            //different join types within the same offset operation
            p.Add(Clipper.MakePath(new int[] { 100, 0, 340, 0, 340, 200, 100, 200 }));
            pp.AddRange(p);
            co.AddPaths(p, JoinType.Round, EndType.Joined);
            /*
            p = Clipper.TranslatePaths(p, 60, 50);
            pp.AddRange(p);
            co.AddPaths(p, JoinType.Square, EndType.Joined);

            p = Clipper.TranslatePaths(p, 60, 50);
            pp.AddRange(p);
            co.AddPaths(p, JoinType.Round, EndType.Joined);
            */


            co.Execute(5, p);
            pp.AddRange(p);

            /*
            string filename = "../../../inflate.svg";
            SvgWriter svg = new();
            SvgUtils.AddSolution(svg, pp, false);
            SvgUtils.AddCaption(svg, "Beveled join", 100, -27);
            SvgUtils.AddCaption(svg, "Squared join", 160, 23);
            SvgUtils.AddCaption(svg, "Rounded join", 220, 73);
            SvgUtils.SaveToFile(svg, filename, FillRule.EvenOdd, 800, 600, 20);
            ClipperFileIO.OpenFileWithDefaultApp(filename);
            */
            return pp;
        }

        public static void DoRabbit()
        {
            PathsD pd = LoadPathsFromResource("InflateDemo.rabbit.bin");

            PathsD solution = new(pd);
            while (pd.Count > 0)
            {
                // and don't forget to scale the delta offset
                pd = Clipper.InflatePaths(pd, -2.5, JoinType.Round, EndType.Polygon);
                // SimplifyPaths - is not essential but it not only 
                // speeds up the loop but it also tidies the result
                pd = Clipper.SimplifyPaths(pd, 0.25);
                solution.AddRange(pd);
            }

            /*
            string filename = "../../../rabbit.svg";
            SvgWriter svg = new();
            SvgUtils.AddSolution(svg, solution, false);
            SvgUtils.SaveToFile(svg, filename, FillRule.EvenOdd, 450, 720, 10);
            ClipperFileIO.OpenFileWithDefaultApp(filename);
            */
        }

        public static PathsD LoadPathsFromResource(string resourceName)
        {
            using Stream stream = Assembly.GetExecutingAssembly().
              GetManifestResourceStream(resourceName);
            if (stream == null) return new PathsD();
            using BinaryReader reader = new(stream);
            int len = reader.ReadInt32();
            PathsD result = new(len);
            for (int i = 0; i < len; i++)
            {
                int len2 = reader.ReadInt32();
                PathD p = new(len2);
                for (int j = 0; j < len2; j++)
                {
                    long X = reader.ReadInt64();
                    long Y = reader.ReadInt64();
                    p.Add(new PointD(X, Y));
                }
                result.Add(p);
            }
            return result;
        }

        public static PathsD DoVariableOffset()
        {
            //Paths64 p = new() { Clipper.MakePath(new int[] { 0, 50, 20, 50, 40, 50, 60, 50, 80, 50, 100, 50 }) };
            //Paths64 p = new() { Clipper.MakePath(new int[] { 100, 0, 340, 0, 340, 200, 100, 200 }) };
            PathD p = Clipper.MakePath(new double[] { 0, 0, 200, 0, 200, 200, 0, 200 });
            PathsD solution = new();
            /*
            ClipperOffset co = new();
            co.AddPaths(p, JoinType.Square, EndType.Butt);
            co.Execute(
              delegate (Path64 path, PathD path_norms, int currPt, int prevPt)
              { return currPt * currPt + 10; }, solution);
            */

            p = Clipper.TrimCollinear(p, 0);
            PathsD pp = new() { p };

            while (pp.Count > 0)
            {
                // and don't forget to scale the delta offset
                pp = Clipper.InflatePaths(pp, -99, JoinType.Miter, EndType.Polygon);
                // SimplifyPaths - is not essential but it not only 
                // speeds up the loop but it also tidies the result
                pp = Clipper.SimplifyPaths(pp, 0.025);
                solution.AddRange(pp);
            }

            /*
            string filename = "../../../variable_offset.svg";
            SvgWriter svg = new();
            SvgUtils.AddOpenSubject(svg, p);
            SvgUtils.AddSolution(svg, solution, true);
            SvgUtils.SaveToFile(svg, filename, FillRule.EvenOdd, 500, 500, 60);
            ClipperFileIO.OpenFileWithDefaultApp(filename);
            */
            //p.AddRange(solution);
            return solution;
        }

        public static PathsD DoUnion()
        {
            //Paths64 p = new() { Clipper.MakePath(new int[] { 0, 50, 20, 50, 40, 50, 60, 50, 80, 50, 100, 50 }) };
            //Paths64 p = new() { Clipper.MakePath(new int[] { 100, 0, 340, 0, 340, 200, 100, 200 }) };
            //PathD p = Clipper.MakePath(new double[] { 0, 0, 200, 0, 200, 200, 0, 200 });
            const int size = 10;
            const int w = 800, h = 600;
            PathD shape = Clipper.MakePath(new double[] { size, 0, size * 2, size, size, size * 2, 0, size });
            PathsD subjects = new(), solution;
            System.Random rand = new();
            for (int i = 0; i < h / size / 2; ++i)
            {
                for (int j = 0; j < w / size; ++j)
                {
                    if (rand.Next(7) != 0) subjects.Add(shape);
                    if ((j & 1) == 0)
                        shape = Clipper.TranslatePath(shape, size, size);
                    else
                        shape = Clipper.TranslatePath(shape, size, -size);
                }
                shape = Clipper.TranslatePath(shape, (-w / size) * size, size * 2);
            }

            FillRule fillrule = FillRule.NonZero;
            solution = Clipper.Union(subjects, fillrule);
            return solution;
        }
#endif

        public static PathsD Union(List<Vector2> vertices)
        {
            int cnt = vertices.Count;
            PathD res = new PathD(cnt);
            for (int i = 0; i < cnt; i++)
            {
                res.Add(new PointD(vertices[i].x, vertices[i].y));
            }

            res = Clipper2Lib.Clipper.TrimCollinear(res, 7);
            PathsD pp = new() { res };

            PathsD solution;
            solution = Clipper2Lib.Clipper.Union(pp, FillRule.NonZero);

            for (int j = 0; j < solution.Count; j++)
            {
                List<Vector3> v = new List<Vector3>();
                for (int i = 0; i < solution[j].Count; i++)
                {
                    PathD pathList = solution[j];
                }
            }
            //solution = Clipper.Union(solution, FillRule.NonZero);

            return solution;
        }

        public static ClipperLib_Paths SimplifyPolygon(List<Vector2> vertices)
        {
            int cnt = vertices.Count;
            ClipperLib_Path res = new ClipperLib_Path(cnt);
            for (int i = 0; i < cnt; i++)
            {
                res.Add(new ClipperLib.IntPoint(vertices[i].x * 100, vertices[i].y * 100));
            }

            ClipperLib_Paths pa = ClipperLib.Clipper.SimplifyPolygon(res, ClipperLib.PolyFillType.pftNonZero);
            return pa;
        }
    }
}