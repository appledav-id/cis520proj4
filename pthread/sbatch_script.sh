#!/bin/bash -l
## A Sample slurm script created by Kyle Hutson
##
## Note: Usually a '#" at the beginning of the line is ignored. However, in
## the case of slurm, lines beginning with #SBATCH are commands for slurm itself, so
## I have taken the convention here of starting *every* line with a '#', just
## Delete the first one if you want to use that line, and then modify it to
## your own purposes. The only exception here is the first line, which *must*
## be #!/bin/bash (or another valid shell).

## Specify the amount of RAM needed _per_core_. Default is 1G
#SBATCH --mem-per-cpu=768M

## Specify the maximum runtime. Default is 1 hour (1:00:00)
#SBATCH --time=0:10:00   # Use the form DD-HH:MM:SS
#SBATCH --partition=killable.q   # Job may run as killable on owned nodes

## Tell slurm how many cores to use, and how many nodes to use
## Jobs requesting 16 or fewer cores tend to get scheduled
## fairly quickly. If you need a job that requires more than that, you might
## benefit from emailing us at beocat@cis.ksu.edu to see how we can assist in
## getting your job scheduled in a reasonable amount of time.
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1

## Name my job, to make it easier to find in the queue
#SBATCH --job-name=mls_pthread

## Send email when a job begins, fails, or ends
#SBATCH --mail-type=ALL   # same as =BEGIN,FAIL,END

## Email address to send the email to based on the above line.
#SBATCH --mail-user=msinclair@ksu.edu

./pthread