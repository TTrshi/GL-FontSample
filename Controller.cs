using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Controller : MonoBehaviour
{
    [SerializeField] private Transform target;
    [SerializeField] private float transformForwardZ = 0.3f;
    [SerializeField] private float angle = 0.0f;
    [SerializeField] private float targetAngle = 0.0f;

    void Start()
    {
        if (!target) target = Camera.main.transform;

        TransformUpdate();
    }

    private void LateUpdate()
    {
        TransformUpdate();
    }

    void TransformUpdate()
    {
        angle = Vector3.SignedAngle(target.transform.forward, transform.position - target.transform.position, Vector3.up);
        float angle_threshold = 60.0f;
        if (Mathf.Abs(angle) < angle_threshold) targetAngle = angle;
        angle = Mathf.Lerp(angle, targetAngle, 0.005f);
        transform.position = target.position + Quaternion.AngleAxis(angle, Vector3.up) * target.transform.forward * transformForwardZ;
        transform.rotation = Quaternion.LookRotation(transform.position - target.transform.position);
    }
}
