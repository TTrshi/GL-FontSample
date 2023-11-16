using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BootApp : MonoBehaviour
{
    [SerializeField] private string companyName = "com.Sunmax.";
    [SerializeField] const string EXTRA_KEY = "TEST";
    const string ACTIVITY = "com.unity3d.player.UnityPlayerActivity";

   public void LaunchApplicatiion(string productName, string value)
    {
        var packageName = companyName + productName;

        using (AndroidJavaClass unityPlayer = new AndroidJavaClass("com.unity3d.player.UnityPlayer"))
        {
            using (AndroidJavaObject activity = unityPlayer.GetStatic<AndroidJavaObject>("currentActivity"))
            {
                AndroidJavaObject Intent = new AndroidJavaObject("android.content.Intent");
                Intent.Call<AndroidJavaObject>("setAction", "android.intent.action.VIEW");
                Intent.Call<AndroidJavaObject>("setClassName", packageName, ACTIVITY);
                Intent.Call<AndroidJavaObject>("putExtra", EXTRA_KEY, value); //‡B

                activity.Call("startActivity", Intent);
            }
        }
    }

    private string GetTextValue(string extraKey)
    {
        using (var player = new AndroidJavaClass("com.unity3d.player.UnityPlayer"))
        using (var activity = player.GetStatic<AndroidJavaObject>("currentActivity"))
        using (var intent = activity.Call<AndroidJavaObject>("getIntent"))
        {
            return intent.Call<string>("getStringExtra", extraKey);
        }
    }

    // Start is called before the first frame update
    void Start()
    {
        //GetTextValue(EXTRA_KEY);
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
