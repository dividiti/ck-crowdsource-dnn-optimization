[![Build Status](https://travis-ci.org/dividiti/crowdsource-video-experiments-on-android.svg?branch=master)](https://travis-ci.org/dividiti/crowdsource-video-experiments-on-android)


Introduction
============

This [CK-powered] (http://github.com/ctuning/ck) open-source cross-platform desktop demo app (at least, for Linux and Windows) based on QT to recognize images via Caffe or TensorFlow, and send reports (timing/correctness/etc) to CK.

You can download this app from the [Google Play Store](https://play.google.com/store/apps/details?id=openscience.crowdsource.video.experiments). 

You can also find public results at [Live CK repo](http://cknowledge.org/repo)!

Public scenarios are prepared using this [CK GitHub repo](https://github.com/ctuning/ck-crowd-scenarios).
Caffe libraries are generated using [CK-Caffe framework](https://github.com/dividiti/ck-caffe).

Current scenarios include multi-dimensional and multi-objective
optimization of benchmarks and real workloads such as 
Caffe, TensorFlow and other DNN frameworks in terms
of performance, accuracy, energy, memory footprint, cost, etc.

See our [vision paper](http://dx.doi.org/10.1145/2909437.2909449).

Related projects:
* hhttps://github.com/dividiti/crowdsource-video-experiments-on-android/issues

License
=======
* Permissive 3-clause BSD license. (See `LICENSE.txt` for more details).

Minimal requirements
====================
Linux, Windows or MacOS operation system

Preparation to run
====================
To make application run you have to install at least one package from each of three categories: dnn-proxy, caffemodel, imagenet dataset.

* dnn-proxy packages:
```
$ ck install package:dnn-proxy-caffe-cpu
$ ck install package:dnn-proxy-caffe-opencl
$ ck install package:dnn-proxy-caffe-cuda
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
To make sure that all is ready, run program caffe-classification. It should compile and run without errors.
```
$ ck compile program:caffe-classification
$ ck run program:caffe-classification
```

## Running the app
After source code built, executable file lays down into _bin_ directory inside the project directory.

Create _app.conf_ file inside the _bin_ directory, which contains info about your local CK installation:
```
[General]
ck_exe_name=./ck
ck_bin_path=/path/to/your/ck/bin
ck_repos_path=~/CK
```
Or you can copy dummy file _src/app.conf.example_ into _bin_ and use it as template.

### Windows note:
Use linux-style paths or double slashes in Windows paths when write config file, e.g.: `ck_bin_path=c:/ck/bin` or `ck_bin_path=c:\\ck\\bin`.

*Don't forget about vc_redist.x64.exe or vc_redist.x86.exe!:*
You have to force user to install VC-redist when build with MS-related version of Qt. Download installer from 
[here](http://www.microsoft.com/en-us/download/details.aspx?id=48145) or google for 'visual studio 2015 runtime'.

If program does not start from _bin_ folder and claims something about *dll* or *plugin*, 
it may be because of Qt bin directory is not in your PATH. Add it to the PATH variable, 
or run script `make_redist.bat` and copy all the dll files and pulgin folders from directory _redist_ to _bin_.

Authors
=======
* Daniil Efremov
* Grigori Fursin (original crowd-tuner: https://github.com/ctuning/crowdsource-experiments-using-android-devices)
* Anton Lokhmotov

Privacy Policy
==============

This application requires access to your Camera to let you
capture images, recognize them and collect various performance 
statistics. Note that, by default, no images are sent to public servers!
Only if misprediction happens, you are encouraged but not obliged (!)
to submit incorrectly recognized image with the correct label 
to the public server to help the community enhance existing
data sets with new images!

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
