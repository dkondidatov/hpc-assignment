Slurm
=========

The role installs and configures Slurm cluster.

Requirements
------------

Roles:
- lustre-client
- munge
- openmpi

Validation
----------
```bash
# On Master node
systemctl status slurmctld
systemctl status slurmdbd
sinfo
srun --nodes=3 --ntasks=3 --chdir /tmp hostname
```

Role Variables
--------------

|Variable|Description|
|---|---|
|lustre_mount_path|Path to shared NFS folder|
|slurmdbd_port|Port will be used by slurmdb daemon|
|slurm_prefix|Prefix for installation|
|slurm_temp_download_dir|Path to the temporary directory for archive downloading|
|slurm_runstate_dir|Path to the "run" directory|
|slurm_config_dir|Path to the "configs" directory|
|slurm_logs_dir|Path to the "log" directory|
|slurm_spool_dir|Path to the "spool" directory|
|slurm_worker_configure_args|Arguments for the "./configure" script to run on worker nodes|
|slurm_master_configure_args|Arguments for the "./configure" script to run on master nodes|
|slurm_dependencies|Slurm dependencies|

Dependencies
------------

- fauust.mariadb

Example Playbook
----------------

```
---
- hosts:
    - hpc_master_nodes
  become: true
  roles: 
    - fauust.mariadb
  vars: 
    mariadb_innodb_raw: |
      innodb_buffer_pool_size=2048M
      innodb_log_file_size=1024M
      innodb_lock_wait_timeout=900
      max_allowed_packet=16M
    mariadb_port: 3306
    mariadb_bind_address: 127.0.0.1
    mariadb_databases:
      - name: slurm_acct_db
        replicate: false
    mariadb_users: 
      - name: slurm
        password: ***
        priv: "*.*:USAGE/slurm_acct_db.*:ALL"
        state: present

- hosts: 
    - hpc_master_nodes
    - hpc_worker_nodes
  become: true
  tasks:
    - name: 'Import : lustre-client role'
      include_role: 
        name: lustre-client
    
    - name: 'Import : awscli role'
      include_role: 
        name: awscli
    
    - name: 'Import : munge role'
      include_role: 
        name: munge
    
    - name: 'Import : openmpi role'
      include_role: 
        name: openmpi

    - name: 'Import : slurm role'
      vars: 
        slurm_skip_installation: true
      include_role: 
        name: slurm
```

License
-------

BSD

Author Information
------------------

Dmitrii Kondidatov (d.kondidatov@gmail.com)