from distutils.core import setup, Extension
import os
import platform

def main():
    CFLAGS_NAIVE = ['-g', '-Wall', '-std=c99', '-O0', '-DNAIVE']
    LDFLAGS_NAIVE = []
    CFLAGS = ['-g', '-Wall', '-std=c99', '-fopenmp', '-mavx', '-mfma', '-pthread', '-O0']
    LDFLAGS = ['-fopenmp']
    # Use the setup function we imported and set up the modules.
    # You may find this reference helpful: https://docs.python.org/3.6/extending/building.html
    # TODO: YOUR CODE HERE
    
    if platform.system() == 'Linux':
        module = Extension(
            'numc', 
            sources = ['numc.c', 'matrix.c'], 
            libraries=['python3.10'], 
            extra_compile_args=CFLAGS, 
            extra_link_args=LDFLAGS
        )
        module_naive = Extension(
            'numc_naive', 
            sources = ['numc.c', 'matrix.c'], 
            libraries=['python3.10'], 
            extra_compile_args=CFLAGS_NAIVE, 
            extra_link_args=LDFLAGS_NAIVE
        )
    if platform.system() == 'Darwin':
        os.environ['CC'] = 'clang'
        os.environ['LDSHARED'] = 'clang -shared'
        module = Extension(
            'numc', 
            sources = ['numc.c', 'matrix.c'], 
            include_dirs=['/opt/homebrew/Cellar/python@3.11/3.11.6_1/Frameworks/Python.framework/Versions/3.11/include/python3.11'], 
            library_dirs=['/opt/homebrew/Cellar/python@3.11/3.11.6_1/Frameworks/Python.framework/Versions/3.11/lib'], 
            libraries=['python3.11'], 
            extra_compile_args=CFLAGS, 
            extra_link_args=LDFLAGS
        )
    setup(name = 'CS61', version = '1.0', description = 'CS61 Numpy', ext_modules = [module, module_naive])
if __name__ == "__main__":
    main()
