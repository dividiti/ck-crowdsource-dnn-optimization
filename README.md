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

License
=======
* Permissive 3-clause BSD license. (See `LICENSE.txt` for more details).

Prerequisites
=============
* Linux, Windows or MacOS operation system
* [Collective Knowledge Framework](https://github.com/ctuning/ck)
* [Various compilers](https://github.com/ctuning/ck/wiki/Compiler-autotuning#Installing_compilers)

### Ubuntu
```
$ sudo apt-get install qtdeclarative5-dev
$ sudo apt-get install libgl1-mesa-dev
```

### Windows
* Download and install [latest Qt](https://www.qt.io/download-open-source)

Collective Knowledge workflow preparation
=========================================
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
Finally, you need to pull this repo and compile this application as a standard CK program:
```
$ ck pull repo --url=https://github.com/dividiti/ck-crowdsource-dnn-optimization.git
$ ck compile program:dnn-desktop-demo
```

Compiling this app
==================

Execute the following to compile this app and follow questions on the screen if any:
```
$ ck compile program:dnn-desktop-demo
```

CK should detect available compilers and Qt library, and then build this application.

Running this app
===============

Execute the following to run the app:
```
$ ck run program:dnn-desktop-demo
```

Related Publications with long term vision
==========================================

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

* <a href="https://github.com/ctuning/ck/wiki/Publications">All related references with BibTex</a>

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
