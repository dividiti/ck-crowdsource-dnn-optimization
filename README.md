[![logo](https://github.com/ctuning/ck-guide-images/blob/master/logo-powered-by-ck.png)](http://cKnowledge.org)
[![logo](https://github.com/ctuning/ck-guide-images/blob/master/logo-validated-by-the-community-simple.png)](http://cTuning.org)
[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

Introduction
============

This is a [CK-powered](http://github.com/ctuning/ck) QT-based open-source
cross-platform and customizable desktop tool set (at least, for Linux and Windows) 
to benchmark and optimize deep learning engines, models and inputs across diverse HW/SW via
[CK](https://github.com/ctuning/ck). It complements our 
[Android application](https://play.google.com/store/apps/details?id=openscience.crowdsource.video.experiments)
to crowdsource deep learning optimization across mobile and IoT devices
provided by volunteers.

At the moment, it calculates the sustainable rate of continuous image classification via 
[CK-Caffe](https://github.com/dviditi/ck-caffe) or 
[CK-TensorFlow](https://github.com/ctuning/ck-tensorflow) 
DNN engines needed for life object recognition!

See our [vision paper](http://dx.doi.org/10.1145/2909437.2909449).

We validated this app on ARM, Intel and NVidia-based devices 
with Linux and Windows (from laptops, servers and powerful high-end tablets such as Surface Pro 4 
to budget devices with constrained resources such as Raspberry Pi 3 and Odroid-XU3).

License
=======
* Permissive 3-clause BSD license. (See `LICENSE.txt` for more details).

Minimal requirements
====================
* Linux, Windows or MacOS operation system
* [Collective Knowledge Framework](https://github.com/ctuning/ck)
* [Various compilers](https://github.com/ctuning/ck/wiki/Compiler-autotuning#Installing_compilers)

* QT5+ library. You can install it as following:

**Ubuntu:** $ sudo apt-get qtdeclarative5-dev

**Windows:** Download Qt from [official website](https://www.qt.io/download-open-source) and install to C:\Qt (CK will then automatically pick it up)

Troubleshooting
===============
On some Linux-based platforms including odroid and Raspberry Pi you may get a message:
```
/usr/bin/ld: **cannot find -lGL**
```
This means that either you OpenCL drivers are not installed or corrupted. To solve this problem you can either try to reinstall 
your video driver (for example, by reinstalling original NVidia drivers) or installing mesa driver:
```
 $ sudo apt-get install libgl1-mesa-dev
```
You may also try to soft link the required library as described at [StackOverflow](http://stackoverflow.com/questions/18406369).

Preparation to run (object classification)
==========================================
To make application run you have to install [ck-caffe](https://github.com/dividiti/ck-caffe) 
and at least one caffemodel and imagenet dataset.

* ck-caffe:
```
$ ck pull repo:ck-caffe --url=https://github.com/dividiti/ck-caffe
```
* caffe models:
```
$ ck install package:caffemodel-bvlc-alexnet
$ ck install package:caffemodel-bvlc-googlenet
```
* imagenet datasets. **Note:** one dataset consists of two packages: `aux` and `val`, you need to install both of them to use the dataset. More datasets may be added in the future:
```
$ ck install package:imagenet-2012-aux
$ ck install package:imagenet-2012-val-min
```

You also need to compile and be able to run `caffe-classification`:
```
$ ck compile program:caffe-classification
$ ck run program:caffe-classification
```

Finally, you need to pull this repo and compile this application as a standard CK program 
(CK will attempt to detect suitable compiler and installed Qt before compiling this app)

```
$ ck pull repo --url=https://github.com/dividiti/crowdsource-video-experiments-on-desktop
$ ck compile program:dnn-desktop-demo
```

Preparation to run (object detection)
=====================================

For the object detection mode to work, you also need to setup SqueezeDet from CK-Tensorflow. 

First, pull the repo:
```
$ ck pull repo:ck-tensorflow
```

After that, you should be able to successfully run a simple demo:
```
$ ck compile program:squeezedet
$ ck run program:squeezedet --cmd_key=squeezedet
```

(it may ask a couple of questions and install some stuff like a dataset at the first run).

If this works for you, the object detection mode in the UI will be available.

Running the app
===============

Execute the following to run this GUI-based app:
```
$ ck run program:dnn-desktop-demo [parameters]
```

Parameters supported:

1. `--params.fps_update_interval_ms`: integer value in milliseconds, controls the maximum frequency of 'Images per second' (FPS) updates. The actual value of FPS will still be shown accurately, but it will be updated at most once per the given value of milliseconds. The default value is `500`. Set it to `0` to get the real-time speed (it may be hard to read). Example: `ck run program:dnn-desktop-demo --params.fps_update_interval_ms=0`

1. `--params.recognition_update_interval_ms`: integer value in milliseconds, controls the maximum frequency of updates in the object recognition mode. The image and metrics below it will be updated at most once per the given value of milliseconds. The default value is `1000`. Set it to `0` to get the real-time speed (it may be hard to read).  Example: `ck run program:dnn-desktop-demo --params.recognition_update_interval_ms=2000`

Parameter values are saved for the future use. So, next time you call `ck run`, you don't need to specify them again.

Select DNN engine, data set and press start button! Enjoy!


Future work
===========

We plan to extend this application to optimize deep learning (training and predictions) 
across different models and their topologies, data sets, libraries and diverse hardware
while sharing optimization statistics with the community via [public repo](http://cKnowledge.org/repo).

This should allow end-users select the most efficient solution for their tasks
(DNN running on a supercomputer or in a constrained IoT device), and computer engineers
designers deliver next generation of efficient libraries and hardware. 

See our vision papers below for more details.

Related Publications with our long term vision
==============================================

```
@inproceedings{Lokhmotov:2016:OCN:2909437.2909449,
 author = {Lokhmotov, Anton and Fursin, Grigori},
 title = {Optimizing Convolutional Neural Networks on Embedded Platforms with OpenCL},
 booktitle = {Proceedings of the 4th International Workshop on OpenCL},
 series = {IWOCL '16},
 year = {2016},
 location = {Vienna, Austria},
 url = {http://doi.acm.org/10.1145/2909437.2909449},
 acmid = {2909449},
 publisher = {ACM},
 address = {New York, NY, USA},
 keywords = {Convolutional neural networks, OpenCL, collaborative optimization, deep learning, optimization knowledge repository},
} 

@inproceedings{ck-date16,
    title = {{Collective Knowledge}: towards {R\&D} sustainability},
    author = {Fursin, Grigori and Lokhmotov, Anton and Plowman, Ed},
    booktitle = {Proceedings of the Conference on Design, Automation and Test in Europe (DATE'16)},
    year = {2016},
    month = {March},
    url = {https://www.researchgate.net/publication/304010295_Collective_Knowledge_Towards_RD_Sustainability}
}
```

* [All related references with BibTex](https://github.com/ctuning/ck/wiki/Publications)

Testimonials and awards
=======================
* 2015: ARM and the cTuning foundation use CK to accelerate computer engineering: [HiPEAC Info'45 page 17](https://www.hipeac.net/assets/public/publications/newsletter/hipeacinfo45.pdf), [ARM TechCon'16 presentation and demo](http://schedule.armtechcon.com/session/know-your-workloads-design-more-efficient-systems), [public CK repo](https://github.com/ctuning/ck-wa)

Acknowledgments
===============

CK development is coordinated by [dividiti](http://dividiti.com)
and the [cTuning foundation](http://cTuning.org) (non-profit research organization)
We are also extremely grateful to all
volunteers for their valuable feedback and contributions.

Feedback
========
Feel free to engage with our community via this mailing list:
* http://groups.google.com/group/collective-knowledge
