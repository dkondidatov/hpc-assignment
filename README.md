# hpc-assignment

This repo contains a code for installing and configuring Slurm HPC cluster. Follow the provided step-by-step guide to install the cluster.

Additional documentation could be found on [Official Site](https://slurm.schedmd.com/quickstart_admin.html)

# Step-by-step guide:
## Requirements:
- SSH key created and available to use
- SSH key distributed to all nodes
- Packages are installed:
```bash
- ansible  
- git
```
## Guide:
1) Make sure application dependencies are installed:
```bash
sudo apt install ansible git
```
2) Clone repo with roles:
```bash
git clone git@github.com:dkondidatov/hpc-assignment.git
```
3) Rewiew and configure ansible `hpc-assignment/ansible/playbook.yml` and ansible/inventory.yml
4) Install ansible-galaxy requirements:
```bash
ansible-galaxy install -r hpc-assignment/ansible/requirements.yml
```
5) Install ansible playbook: 
```bash
ansible-playbook -i ansible/inventory.yml ansible/playbook.yml --private-key=~/.ssh/<"set your private key here">
```
6) Verify all services are running
```bash
# Run on worker nodes
sudo systemctl status slurmd.service
# Run on master nodes
export SLURM_CONF="/etc/slurm/slurm.conf"
sinfo 
sudo systemctl status slurmctl.service
sudo systemctl status slurmdbd.service
```
7) Build С-code
```bash
mpicc ./hpc-assignment/src/pmm.c -o /mnt/fsx/slurm/pmm_c_v2
```
8) Run sbatch script
```bash
export SLURM_CONF="/etc/slurm/slurm.conf"
sbatch ./hpc-assignment/run_script.sh
```
9) Check the results in `/mnt/fsx/slurm`
