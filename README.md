[![logo](https://github.com/ctuning/ck-guide-images/blob/master/logo-powered-by-ck.png)](http://cKnowledge.org)
[![logo](https://github.com/ctuning/ck-guide-images/blob/master/logo-validated-by-the-community-simple.png)](http://cTuning.org)
[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

Introduction
============

This is a [CK-powered](http://github.com/ctuning/ck) QT-based open-source cross-platform desktop demo app (at least, for Linux and Windows). It recognizes images via Caffe or TensorFlow and sends reports (timing/correctness/etc) to CK.

Public scenarios are prepared using this [CK GitHub repo](https://github.com/ctuning/ck-crowd-scenarios).
Caffe libraries are generated using [CK-Caffe framework](https://github.com/dividiti/ck-caffe).

Current scenarios include multi-dimensional and multi-objective
optimization of benchmarks and real workloads such as 
Caffe, TensorFlow and other DNN frameworks in terms
of performance, accuracy, energy, memory footprint, cost, etc.

See our [vision paper](http://dx.doi.org/10.1145/2909437.2909449).

Related projects:
* hhttps://github.com/dividiti/crowdsource-video-experiments-on-android

License
=======
* Permissive 3-clause BSD license. (See `LICENSE.txt` for more details).

Minimal requirements
====================
Linux, Windows or MacOS operation system

Preparation to run
====================
To make application run you have to install [ck-caffe](https://github.com/dividiti/ck-caffe) and at least one caffemodel and imagenet dataset.

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
$ ck install package:imagenet-2012-val
```
You also need to compile and be able to run `caffe-classification`:
```
$ ck compile program:caffe-classification
$ ck run program:caffe-classification
```
Finally, you need to pull this repo and 'compile' the program (currently, 'compilation' only unpacks one of the prebuilt packages)
```
$ ck pull repo --url=https://github.com/dividiti/crowdsource-video-experiments-on-desktop
$ ck compile program:dnn-desktop-demo
```

## Running the app

Execute the following to run the app:
```
$ ck run program:dnn-desktop-demo
```

Questions/comments/discussions?
===============================
Please subscribe to our mailing lists:
* Open, collaborative and reproducible R&D including knowledge preservation, sharing and reuse:
  http://groups.google.com/group/collective-knowledge
* Software and hardware multi-objective (performance/energy/accuracy/size/reliability/cost)
  benchmarking, autotuning, crowdtuning and run-time adaptation: http://groups.google.com/group/ctuning-discussions

Publications
============
The concepts have been described in the following publications:

* http://arxiv.org/abs/1506.06256 (CPC'15)
* http://bit.ly/ck-date16 (DATE'16)
* http://hal.inria.fr/hal-01054763 (Journal of Scientific Programming'14)
* https://hal.inria.fr/inria-00436029 (GCC Summit'09)

If you found this app useful for your R&D, you are welcome
to reference any of the above publications in your articles
and reports. You can download all above references in one 
BibTex file [here](https://raw.githubusercontent.com/ctuning/ck-guide-images/master/collective-knowledge-refs.bib).

Testimonials and awards
=======================
* 2014: HiPEAC technology transfer award: [HiPEAC TT winners](https://www.hipeac.net/research/technology-transfer-awards/2014)
* 2015: ARM and the cTuning foundation use CK to accelerate computer engineering: [HiPEAC Info'45 page 17](https://www.hipeac.net/assets/public/publications/newsletter/hipeacinfo45.pdf), [ARM TechCon'16 presentation and demo](http://schedule.armtechcon.com/session/know-your-workloads-design-more-efficient-systems), [public CK repo](https://github.com/ctuning/ck-wa)

Acknowledgments
===============

CK development is coordinated by [dividiti](http://dividiti.com)
and the [cTuning foundation](http://cTuning.org) (non-profit research organization)
We are also extremely grateful to all
volunteers for their valuable feedback and contributions.

![logo](https://github.com/ctuning/ck-guide-images/blob/master/logo-validated-by-the-community-simple.png)
