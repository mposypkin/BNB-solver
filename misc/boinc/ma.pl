#!/usr/bin/env perl 
use File::Copy;

# extracts new generation from the existing results

# home directory
my $homed = "/var/lib/boinc/dcsdg/project/ma/";
# source folder to get results
my $sourced = "/var/lib/boinc/dcsdg/project/sample_results";
# target folder to store new wus
my $wusd = "/var/lib/boinc/dcsdg/project/ma/tmp";
# incumbent (best value) file
my $bestf = "/var/lib/boinc/dcsdg/project/ma/best.xml";
# log file
my $logf = "/var/lib/boinc/dcsdg/project/ma/log.ma";
# template file for wu
my $wutmplf = "/var/lib/boinc/dcsdg/project/ma/template.xml";
# boinc application name
my $appname = "molc";
# input template file  name
my $itmplf = "itmpl.gw";
# result template file  name
my $rtmplf = "rtmpl.gw";

my $wuc = 0;

sub writeLog {
  open(LOGF, ">>$logf") or die "can't open log file\n";
  my $s = $_[0];
  my $when = localtime;
  print LOGF "$when: $s\n";
  close(LOGF);
}

sub dieLog {
  writeLog($_[0]);
  die($_[0]);
}

sub writeBest {
  unless(open(BESTF, ">$bestf")){
    dieLog ("can't open $bestf file. terminating ...");
  } else {
    my $s = $_[0];
    print BESTF $s;
    close (BESTF);
  }
}


sub readBest {
  unless(open(BESTF, "$bestf")){
    dieLog ("can't open $bestf file. terminating ...");
  } else {
    my $s = <BESTF>; 
    if($s =~ ".*<f>(.*)<\/f>.*"){
      return $1;
    } else {
      close (BESTF);
      dieLog("illegal incumbent file format");
    }
    close (BESTF);
  }
}

sub makeInput {
  open(TF,  $wutmplf) or dieLog("can't open $wutmplf file");
  @tfile = <TF>;
  close(F1);
  my $s = $_[0];
  my $tstr = "@tfile";
  $tstr =~ s/SOLUTMPL/$s/g;
  my $t = time();
  my $wuname = "$wusd/wu-$t-$wuc";
  $wuc ++;
  open(WU, ">$wuname") or dieLog("can't open $wuname file for writing\n");
  print WU $tstr;
  close(WU);
  writeLog("Created $wuname input file");
}

sub makeInputs {
  while(defined($fn = <$sourced/wu*0>)) {
    $K ++;
    open(F, $fn);
    while(<F>) {
      if($_ =~ ".*<f>(.*)<\/f>.*") {
	unless(-e $bestf){
	  writeBest($_);
	} else {
	  my $bv = readBest();
	  if($1 < $bv) {
	    writeLog("updating incumbent to $1");
	    writeBest($_);
	  }
	}
	makeInput($_);
	unlink($fn);
      }
    }
    close(F);
  }
  unlink glob("$sourced/wu*1");
}

sub submitWu {
  my $fn = @_[0];
  chomp($fn);
  my @lst = split(/\//, $fn);
  my $wun = @lst[$#lst];
  writeLog("Submitting WU $wun");
  my $tgt = `dir_hier_path $wun`;
  chomp($tgt);
  writeLog("Copiing to [$tgt] file");
  copy($fn, $tgt) or dieLog("Copiing $fn => $tgt failed");
  (system("create_work -appname $appname -wu_name $wun -wu_template $itmplf -result_template $rtmplf $wun") == 0) or dieLog("Create work failed");
  writeLog("Submitted WU $wun");
  unlink($fn);
}

sub submitWus {
  while(defined($fn = <$wusd/wu*>)) {
    submitWu($fn); 
  }
}

chdir($homed);
writeLog("Started");
makeInputs();
submitWus();
writeLog("Finished");
