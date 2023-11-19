using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;

public class SoundManager : MonoBehaviour
{
    private Dictionary<string, AudioClip> _audioClipDict = new Dictionary<string, AudioClip>();
    private AudioSource _audioSource;

    public static SoundManager Instance = null;

    void Start()
    {
        Instance = this;
        _audioSource = gameObject.AddComponent<AudioSource>();
        LoadAudioClip("SE");
    }


    private void LoadAudioClip(string directoryPath)
    {
        _audioClipDict = Resources.LoadAll<AudioClip>(directoryPath).ToDictionary(clip => clip.name, clip => clip);
    }

    public void PlaySound(string soundName, bool isLoop = false)
    {
        StopSound();

        var audioClip = _audioClipDict[soundName];
        _audioSource.pitch = 1;
        _audioSource.loop = isLoop;
        _audioSource.clip = audioClip;
        _audioSource.Play();
    }

    public void StopSound()
    {
        _audioSource.Stop();
        _audioSource.clip = null;
    }
}
