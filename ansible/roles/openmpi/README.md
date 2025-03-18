OpenMPI
=========

Role for OpenMPI installation

Requirements
------------

No requirements

Role Variables
--------------

|Variable|Description|
|---|---|
|openmpi_version|Version of OpenMPI package|
|openmpi_package_url| Contains URL with package to download. [Source](https://download.open-mpi.org/release/open-mpi/v5.0/openmpi-5.0.7.tar.gz)|
|openmpi_build_parallelism| Set package building parallelism|

Dependencies
------------

No dependencies

Example Playbook
----------------

```yaml
---
- hosts: localhost
  connection: local
  tasks:
    - name: 'Import : openmpi role'
      include_role: 
        name: openmpi```

License
-------

BSD

Author Information
------------------

Dmitrii Kondidatov (d.kondidatov@gmail.com)
