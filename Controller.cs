using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Controller : MonoBehaviour
{
    float angle = 0.0f;
    float targetAngle = 0.0f;
    const float transformForwardZ = 0.3f;
    const float smoothTime = 0.5f;
    const float angleThreshold = 40.0f;
    float velocity = 0.0f;

    Transform cameraTransform;

    void Start()
    {
        cameraTransform = Camera.main.transform;
    }

    private void LateUpdate()
    {
        FollowUpdate();
    }

    void FollowUpdate()
    {
        Vector3 cameraForward = cameraTransform.transform.forward.normalized;
        Vector3 toTarget = transform.position - cameraTransform.transform.position;
        cameraForward.y = 0.0f;
        toTarget.y = 0.0f;

        angle = Vector3.SignedAngle(cameraForward, toTarget, Vector3.up);
        if (Mathf.Abs(angle) < angleThreshold) targetAngle = angle;
        angle = Mathf.SmoothDampAngle(angle, targetAngle, ref velocity, smoothTime);

        transform.position = cameraTransform.position + Quaternion.AngleAxis(angle, Vector3.up) * (cameraForward * transformForwardZ);
        transform.rotation = Quaternion.LookRotation(transform.position - cameraTransform.transform.position);

    }
}
