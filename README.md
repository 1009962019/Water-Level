# Introduction

> This is a project for water level measurement using computer vison technoloy.  



# Getting Started

## Denpendency

This project uses some third part libraries such as :

- plog
- TensoRT
- OpenCV
- Qt5

So first you may install these dependencies.



## Model

Cause this project uses **Senmantic Segmentation** to segment the water first, you may need to train such a model, and use TensorRT to convert it's format to `.engine`. And then, put these model file into the `/model` dir.

