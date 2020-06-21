
.. Keep this document short & concise,
   linking to external resources instead of including content in-line.
   See 'release/text/readme.html' for the end user read-me.

Changes in the branch
=====================

The intention of this branch is to start modernizing the usage of CMake by using IMPORTED libraries for third party libraries,
instead of using paths.

CMake is fully able to provide something that always links, even in face of several type of changes,
but it needs the libraries to be specified as targets, not paths.
Using targets it's especially important to be able to specify relationships among third party libraries,
which otherwise have the chance to be placed in the wrong order on the link line.

This not only makes the code more resilient, but more logical and less difficult to break,
because everything that a third party library needs is directly tied to the target representing it,
and one just needs to link to it to have something working.
This doesn't only mean the library artifact, but also its include directories and other library dependencies.

One issue that has to be solved is how the imported targets are named.
Using library paths and variables have the convenience that it's more generic, because one doesn't
explicitly name the paths, but uses a variable name which is already provided by the find_package module.
For instance if we were to go from the FindGLEW.cmake provided by Blender,
to the one provided by the system, things will keep working when using variables.
But when using targets and the name GLEW::glew internally, moving to the system defined one which is
GLEW::GLEW would cause a problem.
I would say that there are two ways of solving this:

1. Ensure that the internal names are the same as the ones normally provided by the system, when present.
2. Instead of creating an IMPORTED target inside the Find*.cmake module, create it outside,
   using its include and library variables.

The second way is the one I will implement, because it's more generic and especially set and forget.

Blender
=======

Blender is the free and open source 3D creation suite.
It supports the entirety of the 3D pipeline-modeling, rigging, animation, simulation, rendering, compositing,
motion tracking and video editing.

.. figure:: https://code.blender.org/wp-content/uploads/2018/12/springrg.jpg
   :scale: 50 %
   :align: center


Project Pages
-------------

- `Main Website <http://www.blender.org>`__
- `Reference Manual <https://docs.blender.org/manual/en/latest/index.html>`__
- `User Community <https://www.blender.org/community/>`__

Development
-----------

- `Build Instructions <https://wiki.blender.org/wiki/Building_Blender>`__
- `Code Review & Bug Tracker <https://developer.blender.org>`__
- `Developer Forum <https://devtalk.blender.org>`__
- `Developer Documentation <https://wiki.blender.org>`__


License
-------

Blender as a whole is licensed under the GNU Public License, Version 3.
Individual files may have a different, but compatible license.

See `blender.org/about/license <https://www.blender.org/about/license>`__ for details.
