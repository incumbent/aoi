//
//  aoitest.h
//  ml
//
//  Created by JerryZhou on 15/5/21.
//  Copyright (c) 2015年 JerryZhou. All rights reserved.
//

#ifndef ml_aoitest_h
#define ml_aoitest_h

#include "aoi.h"
#include "simpletest.h"

// 清理掉所有缓存并打印内存日志
void clearalliaoicacheandmemrorystate() {
    iaoicacheclear(imetaof(ifilter));
    iaoicacheclear(imetaof(iunit));
    iaoicacheclear(imetaof(inode));
    iaoicacheclear(imetaof(ireflist));
    iaoicacheclear(imetaof(irefjoint));
    
    iaoimemorystate();
}

// **********************************************************************************
// imeta
SP_SUIT(imeta);

SP_CASE(imeta, imetaget) {
    
    imeta *iobjmeta = imetaget(imetaindex(iobj));
    
    SP_TRUE(iobjmeta != NULL);
    
    SP_EQUAL(iobjmeta->size, sizeof(iobj));
    SP_TRUE(strcmp(iobjmeta->name, "iobj") == 0);
}

typedef struct sp_eg_meta_obj {
    irefdeclare;
    int i, j;
}sp_eg_meta_obj;

// 注册类型
iimplementregister(sp_eg_meta_obj, 0);

SP_CASE(imeta, imetaregister) {
    imeta * meta = imetaof(sp_eg_meta_obj);
    SP_EQUAL(meta->size, sizeof(sp_eg_meta_obj));
    SP_TRUE(strcmp(meta->name, "sp_eg_meta_obj") == 0);
    
    SP_EQUAL(meta->current, 0);
    SP_EQUAL(meta->freed, 0);
    SP_EQUAL(meta->alloced, 0);
    
    sp_eg_meta_obj *obj = iobjmalloc(sp_eg_meta_obj);
    iretain(obj);
    
    SP_EQUAL(meta->current, sizeof(sp_eg_meta_obj) + sizeof(iobj));
    SP_EQUAL(meta->freed, 0);
    SP_EQUAL(meta->alloced, sizeof(sp_eg_meta_obj) + sizeof(iobj));
    
    irelease(obj);
    
    SP_EQUAL(meta->current, 0);
    SP_EQUAL(meta->freed, sizeof(sp_eg_meta_obj) + sizeof(iobj));
    SP_EQUAL(meta->alloced, sizeof(sp_eg_meta_obj) + sizeof(iobj));
}

typedef struct sp_test_cache_clear{
    irefdeclare;
    int n, m;
}sp_test_cache_clear;

// 注册类型
iimplementregister(sp_test_cache_clear, 1);

SP_CASE(imeta, iaoicacheclear) {
    imeta * meta = imetaof(sp_test_cache_clear);
    
    // 初始状态的meta
    SP_EQUAL(meta->current, 0);
    SP_EQUAL(meta->freed, 0);
    SP_EQUAL(meta->alloced, 0);
    
    SP_EQUAL(meta->cache.capacity, 1);
    SP_EQUAL(meta->cache.length, 0);
    
    sp_test_cache_clear *clear = iobjmalloc(sp_test_cache_clear);
    iretain(clear);
    
    int size = sizeof(sp_test_cache_clear) + sizeof(iobj);
    
    // 创建一个对象后的meta
    SP_EQUAL(meta->cache.capacity, 1);
    SP_EQUAL(meta->cache.length, 0);
    
    SP_EQUAL(iobjistype(clear, sp_test_cache_clear), iiok);
    SP_EQUAL(iobjistype(clear, iobj), iino);
    
    SP_EQUAL(meta->current, size);
    SP_EQUAL(meta->freed, 0);
    SP_EQUAL(meta->alloced, size);
    
    // 第一个对象释放后进入缓冲区的meta
    irelease(clear);
    SP_EQUAL(meta->current, size);
    SP_EQUAL(meta->freed, 0);
    SP_EQUAL(meta->alloced, size);
    
    SP_EQUAL(meta->cache.capacity, 1);
    SP_EQUAL(meta->cache.length, 1);
    
    // 从缓冲区拿到一个对象后的meta
    sp_test_cache_clear *newclear = iobjmalloc(sp_test_cache_clear);
    iretain(newclear);
    
    SP_EQUAL(newclear, clear);
    
    SP_EQUAL(meta->current, size);
    SP_EQUAL(meta->freed, 0);
    SP_EQUAL(meta->alloced, size);
    
    SP_EQUAL(meta->cache.capacity, 1);
    SP_EQUAL(meta->cache.length, 0);
    
    // 缓冲区已经没有了，重新创建一个对象的meta
    sp_test_cache_clear *nextclear = iobjmalloc(sp_test_cache_clear);
    iretain(nextclear);
    
    SP_EQUAL(meta->current, 2*size);
    SP_EQUAL(meta->freed, 0);
    SP_EQUAL(meta->alloced, 2*size);
    
    SP_EQUAL(meta->cache.capacity, 1);
    SP_EQUAL(meta->cache.length, 0);
    
    //  把新对象放入缓存后的meta
    irelease(nextclear);
    SP_EQUAL(meta->current, 2*size);
    SP_EQUAL(meta->freed, 0);
    SP_EQUAL(meta->alloced, 2*size);
    
    SP_EQUAL(meta->cache.capacity, 1);
    SP_EQUAL(meta->cache.length, 1);
   
    // 缓冲区已经满了，直接释放一个对象后的meta
    irelease(newclear);
    SP_EQUAL(meta->current, size);
    SP_EQUAL(meta->freed, size);
    SP_EQUAL(meta->alloced, 2*size);
    
    SP_EQUAL(meta->cache.capacity, 1);
    SP_EQUAL(meta->cache.length, 1);
    
    // 释放对象缓冲区后的meta
    iaoicacheclear(meta);
    SP_EQUAL(meta->current, 0);
    SP_EQUAL(meta->freed, 2*size);
    SP_EQUAL(meta->alloced, 2*size);
    
    SP_EQUAL(meta->cache.capacity, 1);
    SP_EQUAL(meta->cache.length, 0);
}

SP_CASE(imeta, iaoiistype) {
    sp_test_cache_clear *clear = iobjmalloc(sp_test_cache_clear);
    iretain(clear);
    
    SP_EQUAL(iobjistype(clear, sp_test_cache_clear), iiok);
    SP_EQUAL(iobjistype(clear, iobj), iino);
    
    // 释放对象
    irelease(clear);
    // 清理缓冲区
    iaoicacheclear(imetaof(sp_test_cache_clear));
}


// **********************************************************************************
// time
SP_SUIT(time);

SP_CASE(time, igetcurnanoandigetcurtick) {
    int64_t nano = igetcurnano();
    int64_t ticks = igetcurtick();
    SP_EQUAL((nano - ticks * 1000 < 1000), 1);
}

SP_CASE(time, igetnextnano) {
    int64_t nano0 = igetnextnano();
    int64_t nano1 = igetnextnano();
    
    SP_EQUAL(nano0 != nano1, 1);
}

// **********************************************************************************
// ipos
SP_SUIT(ipos);

SP_CASE(ipos, idistancepow2) {
    ipos pos1 = {.x=0, .y=0};
    ipos pos2 = {.x=3, .y=4};
    
    SP_EQUAL(25.0, idistancepow2(&pos1, &pos2));
}

// **********************************************************************************
// isize
SP_SUIT(isize);

SP_CASE(isize, nothing) {
    SP_EQUAL(1, 1);
}

// **********************************************************************************
// irect
SP_SUIT(irect);

SP_CASE(irect, irectcontainsTHEself) {
    irect r = {.pos={.x=0,.y=0}, .size={.w=1,.h=1}};
    
    SP_EQUAL(irectcontains(&r, &r), 1);
}

SP_CASE(irect, irectcontainsTHEsub) {
    irect r = {.pos={.x=0,.y=0}, .size={.w=2,.h=2}};
    
    irect r0 = {.pos={.x=0,.y=0}, .size={.w=0,.h=0}};
    
    SP_EQUAL(irectcontains(&r, &r0), 1);
}

SP_CASE(irect, irectcontainsTHEsubNo) {
    irect r = {.pos={.x=0,.y=0}, .size={.w=2,.h=2}};
    
    irect r0 = {.pos={.x=0,.y=0}, .size={.w=3,.h=3}};
    
    SP_EQUAL(irectcontains(&r, &r0), 0);
}

SP_CASE(irect, irectcontainsTHEsubNo2) {
    irect r = {.pos={.x=0,.y=0}, .size={.w=2,.h=2}};
    
    irect r0 = {.pos={.x=1,.y=1}, .size={.w=2,.h=2}};
    
    SP_EQUAL(irectcontains(&r, &r0), 0);
}

SP_CASE(irect, irectcontainsTHEsubNo3) {
    irect r = {.pos={.x=0,.y=0}, .size={.w=2,.h=2}};
    
    irect r0 = {.pos={.x=-1,.y=-1}, .size={.w=2,.h=2}};
    
    SP_EQUAL(irectcontains(&r, &r0), 0);
}

SP_CASE(irect, irectcontainsTHEsubNo4) {
    irect r = {.pos={.x=0,.y=0}, .size={.w=2,.h=2}};
    
    irect r0 = {.pos={.x=-1,.y=-1}, .size={.w=5,.h=5}};
    
    SP_EQUAL(irectcontains(&r, &r0), 0);
}

SP_CASE(irect, irectcontainspoint) {
    irect r = {.pos={.x=0,.y=0}, .size={.w=2,.h=2}};
    ipos p = {.x=0, .y=0};
    
    SP_EQUAL(irectcontainspoint(&r, &p), iiok);
    p.x = 2, p.y = 2;
    SP_EQUAL(irectcontainspoint(&r, &p), iiok);
    p.x = 2, p.y = 0;
    SP_EQUAL(irectcontainspoint(&r, &p), iiok);
    p.x = 0, p.y = 2;
    SP_EQUAL(irectcontainspoint(&r, &p), iiok);
    p.x = 1, p.y = 1;
    SP_EQUAL(irectcontainspoint(&r, &p), iiok);
    
    p.x = 2, p.y = -1;
    SP_EQUAL(irectcontainspoint(&r, &p), iino);
    p.x = 3, p.y = -1;
    SP_EQUAL(irectcontainspoint(&r, &p), iino);
    p.x = -1, p.y = -1;
    SP_EQUAL(irectcontainspoint(&r, &p), iino);
    
    p.x = -1, p.y = 1;
    SP_EQUAL(irectcontainspoint(&r, &p), iino);
    p.x = -1, p.y = 2;
    SP_EQUAL(irectcontainspoint(&r, &p), iino);
    p.x = -1, p.y = 3;
    SP_EQUAL(irectcontainspoint(&r, &p), iino);
}

// **********************************************************************************
// icircle
SP_SUIT(icircle);

SP_CASE(icircle, icircleintersect) {
    icircle c = {.pos={.x=0, .y=0}, .radis=1.0};
    
    SP_EQUAL(c.radis, 1.0);
    
    icircle c0 = {.pos={.x=0, .y=0}, .radis=2.0};
    
    SP_EQUAL(icircleintersect(&c, &c0), 1);
}

SP_CASE(icircle, icircleintersectYES) {
    icircle c = {.pos={.x=0, .y=0}, .radis=1.0};
    
    SP_EQUAL(c.radis, 1.0);
    
    icircle c0 = {.pos={.x=0, .y=3}, .radis=2.0};
    
    SP_EQUAL(icircleintersect(&c, &c0), 1);
}

SP_CASE(icircle, icircleintersectNo) {
    icircle c = {.pos={.x=0, .y=0}, .radis=1.0};
    
    SP_EQUAL(c.radis, 1.0);
    
    icircle c0 = {.pos={.x=3, .y=3}, .radis=2.0};
    
    SP_EQUAL(icircleintersect(&c, &c0), 0);
}

SP_CASE(icircle, icirclecontains) {
    icircle c = {.pos={.x=0, .y=0}, .radis=2.0};
    
    SP_EQUAL(icirclecontains(&c, &c), 1);
}

SP_CASE(icircle, icirclecontainsYES) {
    icircle c = {.pos={.x=0, .y=0}, .radis=2.0};
    
    icircle c0 = {.pos={.x=0, .y=0}, .radis=1.0};
    
    SP_EQUAL(icirclecontains(&c, &c0), 1);
}

SP_CASE(icircle, icirclecontainsYES1) {
    icircle c = {.pos={.x=0, .y=0}, .radis=2.0};
    
    SP_EQUAL(icirclecontains(&c, NULL), 1);
}

SP_CASE(icircle, icirclecontainsNO) {
    icircle c = {.pos={.x=0, .y=0}, .radis=2.0};
    
    SP_EQUAL(icirclecontains(NULL, &c), 0);
}

SP_CASE(icircle, icirclecontainsNO1) {
    icircle c = {.pos={.x=0, .y=0}, .radis=2.0};
    icircle c0 = {.pos={.x=0, .y=0}, .radis=3.0};
    
    SP_EQUAL(icirclecontains(&c, &c0), 0);
}

SP_CASE(icircle, icirclecontainsNO2) {
    icircle c = {.pos={.x=0, .y=0}, .radis=2.0};
    icircle c0 = {.pos={.x=1, .y=1}, .radis=2.0};
    
    SP_EQUAL(icirclecontains(&c, &c0), 0);
}

SP_CASE(icircle, icirclecontainsNO3) {
    icircle c = {.pos={.x=0, .y=0}, .radis=2.0};
    icircle c0 = {.pos={.x=5, .y=5}, .radis=2.0};
    
    SP_EQUAL(icirclecontains(&c, &c0), 0);
}

SP_CASE(icircle, icirclerelation) {
    icircle c = {.pos={.x=0, .y=0}, .radis=2.0};
    icircle c0 = {.pos={.x=0, .y=0}, .radis=1.0};
    icircle c1 = {.pos={.x=1, .y=0}, .radis=1.0};
    icircle c2 = {.pos={.x=3, .y=0}, .radis=1.0};
    
    SP_EQUAL(icirclerelation(&c, &c), EnumCircleRelationAContainsB);
    SP_EQUAL(icirclerelation(&c, &c0), EnumCircleRelationAContainsB);
    SP_EQUAL(icirclerelation(&c0, &c), EnumCircleRelationBContainsA);
    SP_EQUAL(icirclerelation(&c0, &c1), EnumCircleRelationIntersect);
    SP_EQUAL(icirclerelation(&c0, &c2), EnumCircleRelationNoIntersect);
}

SP_CASE(icircle, icirclecontainspoint) {
    icircle c = {.pos={.x=0, .y=0}, .radis=2.0};
    ipos p = {.x=0, .y=0};
    
    SP_EQUAL(icirclecontainspoint(&c, &p), iiok);
    
    p.x = 2, p.y = 0;
    SP_EQUAL(icirclecontainspoint(&c, &p), iiok);
    
    p.x = 3, p.y = 0;
    SP_EQUAL(icirclecontainspoint(&c, &p), iino);
}

// **********************************************************************************
// iname
SP_SUIT(iname);

SP_CASE(iname, nothing) {
    iname name = {.name = {'A', 'B', 'C', 0}};
    
    SP_EQUAL(name.name[0], 'A');
    SP_EQUAL(name.name[1], 'B');
    SP_EQUAL(name.name[2], 'C');
    SP_EQUAL(name.name[3], 0);
}

// **********************************************************************************
// iref
SP_SUIT(iref);

SP_CASE(iref, iretainANDirelease) {
    iref *ref = iobjmalloc(iref);
    
    SP_EQUAL(ref->ref, 0);
    
    iretain(ref);
    
    SP_EQUAL(ref->ref, 1);
    
    iretain(ref);
    
    SP_EQUAL(ref->ref, 2);
    
    irelease(ref);
    
    SP_EQUAL(ref->ref, 1);
    
    irelease(ref);
}

static int facereffreecc = 0;
static void fake_ref_free(iref *ref) {
    iobjfree(ref);
    ++facereffreecc;
}

SP_CASE(iref, irelease) {
    iref *ref = iobjmalloc(iref);
    ref->free = fake_ref_free;
    
    SP_EQUAL(ref->ref, 0);
    
    iretain(ref);
    
    SP_EQUAL(ref->ref, 1);
    
    iretain(ref);
    
    SP_EQUAL(ref->ref, 2);
    
    irelease(ref);
    
    SP_EQUAL(ref->ref, 1);
    
    SP_EQUAL(facereffreecc, 0);
    irelease(ref);
    SP_EQUAL(facereffreecc, 1);
}

// **********************************************************************************
// ireflist
SP_SUIT(ireflist);

SP_CASE(ireflist, ireflistmake) {
    ireflist *list = ireflistmake();
    
    ireflistfree(list);
}

SP_CASE(ireflist, ireflistlen) {
    ireflist *list = ireflistmake();
    
    SP_EQUAL(ireflistlen(list), 0);
    
    ireflistfree(list);
}

SP_CASE(ireflist, ireflistfirst) {
    ireflist *list = ireflistmake();
    
    SP_EQUAL(ireflistfirst(list), NULL);
    
    ireflistfree(list);
}

SP_CASE(ireflist, ireflistadd) {
    ireflist *list = ireflistmake();
    
    iref *ref = iobjmalloc(iref);
    iretain(ref);
    SP_EQUAL(ref->ref, 1);
    
    ireflistadd(list, ref);
    
    SP_EQUAL(ref->ref, 2);
    
    SP_EQUAL(ireflistlen(list), 1);
    
    ireflistfree(list);
    
    SP_EQUAL(ref->ref, 1);
    
    irelease(ref);
}

SP_CASE(ireflist, ireflistaddANDireflistfirst) {
    ireflist *list = ireflistmake();
    
    iref *ref = iobjmalloc(iref);
    iretain(ref);
    SP_EQUAL(ref->ref, 1);
    
    irefjoint *joint = ireflistadd(list, ref);
    
    SP_EQUAL(ref->ref, 2);
    
    SP_EQUAL(ireflistlen(list), 1);
    
    SP_EQUAL(ireflistfirst(list), joint);
    
    ireflistfree(list);
    
    SP_EQUAL(ref->ref, 1);
    
    irelease(ref);
}

SP_CASE(ireflist, ireflistfind) {
    ireflist *list = ireflistmake();
    
    iref *ref = iobjmalloc(iref);
    iretain(ref);
    SP_EQUAL(ref->ref, 1);
    
    irefjoint *joint = ireflistadd(list, ref);
    
    SP_EQUAL(ref->ref, 2);
    
    SP_EQUAL(ireflistlen(list), 1);
    
    SP_EQUAL(ireflistfind(list, ref), joint);
    
    ireflistfree(list);
    
    SP_EQUAL(ref->ref, 1);
    
    irelease(ref);
}

SP_CASE(ireflist, ireflistaddjoint) {
    ireflist *list = ireflistmake();
    
    iref *ref = iobjmalloc(iref);
    iretain(ref);
    SP_EQUAL(ref->ref, 1);
    
    irefjoint *joint = ireflistadd(list, ref);
    
    SP_EQUAL(ref->ref, 2);
    
    SP_EQUAL(ireflistlen(list), 1);
    
    SP_EQUAL(ireflistfind(list, ref), joint);
    
    irefjoint *add = irefjointmake(ref);
    
    ireflistaddjoint(list, add);
    
    SP_EQUAL(ireflistlen(list), 2);
    
    SP_EQUAL(ireflistfind(list, ref), add);
    
    SP_EQUAL(ireflistfirst(list), add);
    
    ireflistfree(list);
    
    SP_EQUAL(ref->ref, 1);
    
    irelease(ref);
}

SP_CASE(ireflist, ireflistremovejoint) {
    ireflist *list = ireflistmake();
    
    iref *ref = iobjmalloc(iref);
    iretain(ref);
    SP_EQUAL(ref->ref, 1);
    
    irefjoint *joint = ireflistadd(list, ref);
    
    SP_EQUAL(ref->ref, 2);
    
    SP_EQUAL(ireflistlen(list), 1);
    
    SP_EQUAL(ireflistfind(list, ref), joint);
    
    irefjoint *add = irefjointmake(ref);
    
    SP_EQUAL(add->list, NULL);
    
    ireflistaddjoint(list, add);
    
    SP_EQUAL(add->list, list);
    
    SP_EQUAL(ireflistlen(list), 2);
    
    SP_EQUAL(ireflistfind(list, ref), add);
    
    SP_EQUAL(ireflistfirst(list), add);
    
    ireflistremovejoint(list, add);
    
    SP_EQUAL(ireflistlen(list), 1);
    
    SP_EQUAL(ireflistfind(list, ref), joint);
    
    SP_EQUAL(ireflistfirst(list), joint);
    
    irefjointfree(add);
    
    ireflistfree(list);
    
    SP_EQUAL(ref->ref, 1);
    
    irelease(ref);
}


SP_CASE(ireflist, ireflistremove) {
    ireflist *list = ireflistmake();
    
    iref *ref = iobjmalloc(iref);
    iretain(ref);
    SP_EQUAL(ref->ref, 1);
    
    irefjoint *joint = ireflistadd(list, ref);
    
    SP_EQUAL(ref->ref, 2);
    
    SP_EQUAL(ireflistlen(list), 1);
    
    SP_EQUAL(ireflistfind(list, ref), joint);
    
    irefjoint *add = irefjointmake(ref);
    
    ireflistaddjoint(list, add);
    
    SP_EQUAL(ireflistlen(list), 2);
    
    SP_EQUAL(ireflistfind(list, ref), add);
    
    SP_EQUAL(ireflistfirst(list), add);
    
    
    SP_EQUAL(add->list, list);
    
    ireflistremovejoint(list, add);
    
    SP_EQUAL(add->list, NULL);
    
    
    SP_EQUAL(ireflistlen(list), 1);
    
    SP_EQUAL(ireflistfind(list, ref), joint);
    
    SP_EQUAL(ireflistfirst(list), joint);
    
    ireflistremove(list, ref);
    
    SP_EQUAL(ireflistlen(list), 0);
    
    SP_EQUAL(ireflistfind(list, ref), NULL);
    
    SP_EQUAL(ireflistfirst(list), NULL);
    
    irefjointfree(add);
    
    ireflistfree(list);
    
    SP_EQUAL(ref->ref, 1);
    
    irelease(ref);
}

SP_CASE(ireflist, ireflistremoveall) {
    ireflist *list = ireflistmake();
    
    iref *ref = iobjmalloc(iref);
    iretain(ref);
    SP_EQUAL(ref->ref, 1);
    
    irefjoint *joint = ireflistadd(list, ref);
    
    SP_EQUAL(ref->ref, 2);
    
    SP_EQUAL(ireflistlen(list), 1);
    
    SP_EQUAL(ireflistfind(list, ref), joint);
    
    irefjoint *add = irefjointmake(ref);
    
    ireflistaddjoint(list, add);
    
    SP_EQUAL(ireflistlen(list), 2);
    
    SP_EQUAL(ireflistfind(list, ref), add);
    
    SP_EQUAL(ireflistfirst(list), add);
    
    ireflistremoveall(list);
    
    SP_EQUAL(ireflistlen(list), 0);
    
    SP_EQUAL(ireflistfind(list, ref), NULL);
    
    SP_EQUAL(ireflistfirst(list), NULL);
    
    ireflistfree(list);
    
    SP_EQUAL(ref->ref, 1);
    
    irelease(ref);
}

// **********************************************************************************
// irefcache
SP_SUIT(irefcache);

static int fakecachenewcc = 0;

void fake_cache_free(iref *ref) {
    --fakecachenewcc;
    iobjfree(ref);
}

iref* fake_cache_new() {
    ++fakecachenewcc;
    iref *ref= iobjmalloc(iref);
    ref->free = fake_cache_free;
    iretain(ref);
    return ref;
}

SP_CASE(irefcache, irefcachemake) {
    irefcache *cache = irefcachemake(2, fake_cache_new);
    
    SP_EQUAL(cache->capacity, 2);
    
    SP_EQUAL(cache->newentry, fake_cache_new);
    
    irefcachefree(cache);
}

SP_CASE(irefcache, irefcachepoll) {
    irefcache *cache = irefcachemake(1, fake_cache_new);
    
    SP_EQUAL(cache->capacity, 1);
    
    SP_EQUAL(cache->newentry, fake_cache_new);
    
    SP_EQUAL(fakecachenewcc, 0);
    iref *ref = irefcachepoll(cache);
    SP_EQUAL(fakecachenewcc, 1);
    iref *ref0 = irefcachepoll(cache);
    SP_EQUAL(fakecachenewcc, 2);
    irefcachepush(cache, ref);
    SP_EQUAL(fakecachenewcc, 2);
    irefcachepush(cache, ref0);
    SP_EQUAL(fakecachenewcc, 1);
    
    iref *ref1 = irefcachepoll(cache);
    SP_EQUAL(fakecachenewcc, 1);
    SP_EQUAL(ref1, ref);
    
    irefcachepush(cache, ref1);
    
    irefcachefree(cache);
    
    SP_EQUAL(fakecachenewcc, 0);
}

SP_CASE(irefcache, irefcachepush) {
    irefcache *cache = irefcachemake(1, fake_cache_new);
    
    SP_EQUAL(cache->capacity, 1);
    
    SP_EQUAL(cache->newentry, fake_cache_new);
    
    SP_EQUAL(fakecachenewcc, 0);
    iref *ref = irefcachepoll(cache);
    SP_EQUAL(fakecachenewcc, 1);
    iref *ref0 = irefcachepoll(cache);
    SP_EQUAL(fakecachenewcc, 2);
    irefcachepush(cache, ref);
    SP_EQUAL(fakecachenewcc, 2);
    irefcachepush(cache, ref0);
    SP_EQUAL(fakecachenewcc, 1);
    
    iref *ref1 = irefcachepoll(cache);
    SP_EQUAL(fakecachenewcc, 1);
    SP_EQUAL(ref1, ref);
    
    irefcachepush(cache, ref1);
    
    irefcachefree(cache);
    
    SP_EQUAL(fakecachenewcc, 0);
}

SP_CASE(irefcache, irefcachefree) {
    irefcache *cache = irefcachemake(1, fake_cache_new);
    
    SP_EQUAL(cache->capacity, 1);
    
    SP_EQUAL(cache->newentry, fake_cache_new);
    
    SP_EQUAL(fakecachenewcc, 0);
    iref *ref = irefcachepoll(cache);
    SP_EQUAL(fakecachenewcc, 1);
    iref *ref0 = irefcachepoll(cache);
    SP_EQUAL(fakecachenewcc, 2);
    irefcachepush(cache, ref);
    SP_EQUAL(fakecachenewcc, 2);
    irefcachepush(cache, ref0);
    SP_EQUAL(fakecachenewcc, 1);
    
    iref *ref1 = irefcachepoll(cache);
    SP_EQUAL(fakecachenewcc, 1);
    SP_EQUAL(ref1, ref);
    
    irefcachepush(cache, ref1);
    
    irefcachefree(cache);
    
    SP_EQUAL(fakecachenewcc, 0);
}

// **********************************************************************************
// icode
SP_SUIT(icode);

SP_CASE(icode, nothing) {
    icode code = {.code={'A','B', 0}, .pos={.x=0, .y=0}};
    
    SP_EQUAL(code.code[0], 'A');
}

// **********************************************************************************
// iuserdata
SP_SUIT(iuserdata);

SP_CASE(iuserdata, nothing) {
    iuserdata u = {.u1=0, .up1=NULL};
    
    SP_EQUAL(u.u1, 0);
    
    SP_EQUAL(u.up1, NULL);
}

// **********************************************************************************
// iunit
SP_SUIT(iunit);

SP_CASE(iunit, imakeunit) {
    iunit *unit = imakeunit(0, 0, 0);
    
    SP_EQUAL(unit->id, 0);
    
    ifreeunit(unit);
}

SP_CASE(iunit, ifreeunit) {
    iunit *unit = imakeunit(0, 0, 0);
    
    SP_EQUAL(unit->id, 0);
    
    ifreeunit(unit);
}

SP_CASE(iunit, ifreeunitlist) {
    iunit *unit = imakeunit(0, 0, 0);
    
    SP_EQUAL(unit->id, 0);
    
    ifreeunitlist(unit);
}

// **********************************************************************************
// inode
SP_SUIT(inode);

SP_CASE(inode, nothing) {
    inode *node = imakenode();
    ifreenodekeeper(node);
    
    SP_EQUAL(1, 1);
}


// **********************************************************************************
// imap
SP_SUIT(imap);

imap *map = NULL; // {.x=0, .y=0}, {.w=8, .h=8}, 3
// 是一个左闭右开区间
// AAA: [(0,0) -- (1.0, 1.0))
// AAD: [(1.0, 1.0) --- (2.0, 2.0))
/**
 MAPDETAIL 1" size=4
 |B| D|
 |_____
 |A| C|
 |_____
 MAPDETAIL 2" size=2
 |BB| BD| DB| DD|
 |________________
 |BA| BC| DA| DC|
 |________________
 |AB| AD| CB| CD|
 |________________
 |AA| AC| CA| CC|
 |________________
 MAPDETAIL 3" size=1
 |BBB| BBD| BDB| BDD| DBB| DBD| DDB| DDD|
 |_______________________________________
 |BBA| BBC| BDA| BDC| DBA| DBC| DDA| DDC|
 |_______________________________________
 |BAB| BAD| BCB| BCD| DAB| DAD| DCB| DCD|
 |_______________________________________
 |BAA| BAC| BCA| BCC| DAA| DAC| DCA| DCC|
 |_______________________________________
 |ABB| ABD| ADB| ADD| CBB| CBD| CDB| CDD|
 |_______________________________________
 |ABA| ABC| ADA| ADC| CBA| CBC| CDA| CDC|
 |_______________________________________
 |AAB| AAD| ACB| ACD| CAB| CAD| CCB| CCD|
 |_______________________________________
 |AAA| AAC| ACA| ACC| CAA| CAC| CCA| CCC|
 |_______________________________________
  */
void free_test_map() {
    if(map) {
        imapfree(map);
        map = NULL;
    }
}

void make_test_map() {
    free_test_map();
    ipos pos = {.x=0, .y=0};
    isize size = {.w=8, .h=8};
    map = imapmake(&pos, &size, 3);
}

SP_CASE(imap, imapmake) {
    make_test_map();
    
    SP_EQUAL(map->divide, 3);
    SP_EQUAL(map->state.nodecount, 0);
    SP_EQUAL(map->state.leafcount, 0);
    SP_EQUAL(map->state.unitcount, 0);
    
    SP_EQUAL(map->root != NULL, 1);
    SP_EQUAL(map->root->level, 0);
    SP_EQUAL(map->root->code.code[0], 'R');
    SP_EQUAL(map->root->code.code[1], 'O');
    SP_EQUAL(map->root->code.code[2], 'O');
    SP_EQUAL(map->root->code.code[3], 'T');
    
    SP_EQUAL(map->nodesizes[0].w, 8);
    SP_EQUAL(map->nodesizes[0].h, 8);
    
    SP_EQUAL(map->nodesizes[1].w, 4);
    SP_EQUAL(map->nodesizes[1].h, 4);
    
    SP_EQUAL(map->nodesizes[2].w, 2);
    SP_EQUAL(map->nodesizes[2].h, 2);
    
    SP_EQUAL(map->nodesizes[3].w, 1);
    SP_EQUAL(map->nodesizes[3].h, 1);
    
    SP_EQUAL(map->distances[0], 128);
    SP_EQUAL(map->distances[1], 32);
    SP_EQUAL(map->distances[2], 8);
    SP_EQUAL(map->distances[3], 2);
    
    
}

SP_CASE(imap, imapgencode) {
    icode code;
    
    ipos p = {.x = 0, .y = 0};
    
    imapgencode(map, &p, &code);
    
    SP_EQUAL(code.pos.x, p.x);
    SP_EQUAL(code.pos.y, p.y);
    
    SP_EQUAL(code.code[0], 'A');
    SP_EQUAL(code.code[1], 'A');
    SP_EQUAL(code.code[2], 'A');
    SP_EQUAL(code.code[3], 0);
    
    p.x = 1.0;
    p.y = 1.0;
    
    imapgencode(map, &p, &code);
    
    SP_EQUAL(code.code[0], 'A');
    SP_EQUAL(code.code[1], 'A');
    SP_EQUAL(code.code[2], 'D');
    SP_EQUAL(code.code[3], 0);
}

SP_CASE(imap, imapgenpos) {
    
    icode code = {.code={'A', 'A', 'A', 0}};
    
    ipos p = {.x = 0, .y = 0};
    
    imapgenpos(map, &p, &code);
    
    SP_EQUAL(0, p.x);
    SP_EQUAL(0, p.y);
    
    code.code[2] = 'D';
    
    imapgenpos(map, &p, &code);
    
    SP_EQUAL(1.0, p.x);
    SP_EQUAL(1.0, p.y);
}

///简单的单元管理
static int64_t gid = 0;
static iunit* gunits[1000] = {};

#define __preparecnt 20

#define __getmeunit(x, y) imakeunit(gid++, x, y)

#define __hold(u) gunits[u->id] = u; iretain(u)

#define __getunitfor(id) gunits[id]

#define __unhold(id) irelease(gunits[id]); gunits[id] = NULL

#define __setu(xid, xx, xy) do { gunits[xid]->pos.x = xx; gunits[xid]->pos.y = xy; } while(0)

#define __resetu(xid) __setu(xid, 0, 0)

#define __setcode(code, a, b, c) code.code[0] = a; code.code[1] = b; code.code[2] = c

#define __getnode(code, level) imapgetnode(map, &code, level, EnumFindBehaviorAccurate)

// 准备20个在原点的单元
SP_CASE(imap, prepareunit) {
    
    for (int i=0; i<__preparecnt; ++i) {
        iunit *u = __getmeunit(0, 0);
        __hold(u);
        ifreeunit(u);
    }
    
    SP_TRUE(1);
}

SP_CASE(imap, imapaddunit) {
    
    SP_EQUAL(map->divide, 3);
    SP_EQUAL(map->state.nodecount, 0);
    SP_EQUAL(map->state.leafcount, 0);
    SP_EQUAL(map->state.unitcount, 0);
    
    imapaddunit(map, __getunitfor(0));
    
    /**
     |BBB| BBD| BDB| BDD| DBB| DBD| DDB| DDD|
     |_______________________________________
     |BBA| BBC| BDA| BDC| DBA| DBC| DDA| DDC|
     |_______________________________________
     |BAB| BAD| BCB| BCD| DAB| DAD| DCB| DCD|
     |_______________________________________
     |BAA| BAC| BCA| BCC| DAA| DAC| DCA| DCC|
     |_______________________________________
     |ABB| ABD| ADB| ADD| CBB| CBD| CDB| CDD|
     |_______________________________________
     |ABA| ABC| ADA| ADC| CBA| CBC| CDA| CDC|
     |_______________________________________
     |AAB| AAD| ACB| ACD| CAB| CAD| CCB| CCD|
     |_______________________________________
     |AAA:[0]| AAC| ACA| ACC| CAA| CAC| CCA| CCC|
     |_______________________________________
     */
    
    SP_EQUAL(map->divide, 3);
    SP_EQUAL(map->state.nodecount, 3);
    SP_EQUAL(map->state.leafcount, 1);
    SP_EQUAL(map->state.unitcount, 1);
}

SP_CASE(imap, imapremoveunit) {
    SP_EQUAL(map->divide, 3);
    SP_EQUAL(map->state.nodecount, 3);
    SP_EQUAL(map->state.leafcount, 1);
    SP_EQUAL(map->state.unitcount, 1);
    
    imapremoveunit(map, __getunitfor(0));
    
    /**
     |BBB| BBD| BDB| BDD| DBB| DBD| DDB| DDD|
     |_______________________________________
     |BBA| BBC| BDA| BDC| DBA| DBC| DDA| DDC|
     |_______________________________________
     |BAB| BAD| BCB| BCD| DAB| DAD| DCB| DCD|
     |_______________________________________
     |BAA| BAC| BCA| BCC| DAA| DAC| DCA| DCC|
     |_______________________________________
     |ABB| ABD| ADB| ADD| CBB| CBD| CDB| CDD|
     |_______________________________________
     |ABA| ABC| ADA| ADC| CBA| CBC| CDA| CDC|
     |_______________________________________
     |AAB| AAD| ACB| ACD| CAB| CAD| CCB| CCD|
     |_______________________________________
     |AAA| AAC| ACA| ACC| CAA| CAC| CCA| CCC|
     |_______________________________________
     */
    
    SP_EQUAL(map->divide, 3);
    SP_EQUAL(map->state.nodecount, 0);
    SP_EQUAL(map->state.leafcount, 0);
    SP_EQUAL(map->state.unitcount, 0);
}

SP_CASE(imap, complexANDimapaddunitANDimapremoveunitANDimapgetnode) {
    
    SP_EQUAL(map->divide, 3);
    SP_EQUAL(map->state.nodecount, 0);
    SP_EQUAL(map->state.leafcount, 0);
    SP_EQUAL(map->state.unitcount, 0);
    
    imapaddunit(map, __getunitfor(0));
    
    /**
     |BBB| BBD| BDB| BDD| DBB| DBD| DDB| DDD|
     |_______________________________________
     |BBA| BBC| BDA| BDC| DBA| DBC| DDA| DDC|
     |_______________________________________
     |BAB| BAD| BCB| BCD| DAB| DAD| DCB| DCD|
     |_______________________________________
     |BAA| BAC| BCA| BCC| DAA| DAC| DCA| DCC|
     |_______________________________________
     |ABB| ABD| ADB| ADD| CBB| CBD| CDB| CDD|
     |_______________________________________
     |ABA| ABC| ADA| ADC| CBA| CBC| CDA| CDC|
     |_______________________________________
     |AAB| AAD| ACB| ACD| CAB| CAD| CCB| CCD|
     |_______________________________________
     |AAA:[0]| AAC| ACA| ACC| CAA| CAC| CCA| CCC|
     |_______________________________________
     */
    
    SP_EQUAL(map->divide, 3);
    SP_EQUAL(map->state.nodecount, 3);
    SP_EQUAL(map->state.leafcount, 1);
    SP_EQUAL(map->state.unitcount, 1);
    
    __setu(1, 0.5, 0.7);
    __setu(2, 0.99, 0.2);
    
    imapaddunit(map, __getunitfor(1));
    imapaddunit(map, __getunitfor(2));
    
    /**
     |BBB| BBD| BDB| BDD| DBB| DBD| DDB| DDD|
     |_______________________________________
     |BBA| BBC| BDA| BDC| DBA| DBC| DDA| DDC|
     |_______________________________________
     |BAB| BAD| BCB| BCD| DAB| DAD| DCB| DCD|
     |_______________________________________
     |BAA| BAC| BCA| BCC| DAA| DAC| DCA| DCC|
     |_______________________________________
     |ABB| ABD| ADB| ADD| CBB| CBD| CDB| CDD|
     |_______________________________________
     |ABA| ABC| ADA| ADC| CBA| CBC| CDA| CDC|
     |_______________________________________
     |AAB| AAD| ACB| ACD| CAB| CAD| CCB| CCD|
     |_______________________________________
     |AAA:[0, 1, 2]| AAC| ACA| ACC| CAA| CAC| CCA| CCC|
     |_______________________________________
     */
    
    SP_EQUAL(map->divide, 3);
    SP_EQUAL(map->state.nodecount, 3);
    SP_EQUAL(map->state.leafcount, 1);
    SP_EQUAL(map->state.unitcount, 3);
    
    icode code = {.code={'A', 'A', 'A', 0}};
    
    inode *node = imapgetnode(map, &code, 0, EnumFindBehaviorAccurate);
    
    SP_EQUAL(node, map->root);
    
    node = imapgetnode(map, &code, 1, EnumFindBehaviorAccurate);
    
    SP_EQUAL(node != NULL
                     && node->code.code[0] == 'A'
                     && node->code.code[1] == 0, 1);
    
    node = imapgetnode(map, &code, 2, EnumFindBehaviorAccurate);
    
    SP_EQUAL(node != NULL
                     && node->code.code[0] == 'A'
                     && node->code.code[1] == 'A'
                     && node->code.code[2] == 0, 1);
    
    node = imapgetnode(map, &code, 3, EnumFindBehaviorAccurate);
    
    SP_EQUAL(node != NULL
                     && node->code.code[0] == 'A'
                     && node->code.code[1] == 'A'
                     && node->code.code[2] == 'A'
                     && node->code.code[3] == 0, 1);
    
    SP_EQUAL(node, __getunitfor(0)->node);
    SP_EQUAL(node, __getunitfor(1)->node);
    SP_EQUAL(node, __getunitfor(2)->node);
    
    __setu(3, 1, 1);
    
    imapaddunit(map, __getunitfor(3));
    
    SP_EQUAL(map->state.nodecount, 4);
    SP_EQUAL(map->state.leafcount, 2);
    SP_EQUAL(map->state.unitcount, 4);
    
    code.code[2] = 'D';
    
    node = imapgetnode(map, &code, 3, EnumFindBehaviorAccurate);
    
    SP_EQUAL(node != NULL
                     && node->code.code[0] == 'A'
                     && node->code.code[1] == 'A'
                     && node->code.code[2] == 'D'
                     && node->code.code[3] == 0, 1);
    
    SP_EQUAL(node, __getunitfor(3)->node);
    
    code.code[0] = 'D';
    node = imapgetnode(map, &code, 3, EnumFindBehaviorAccurate);
    SP_EQUAL(node, NULL);
    node = imapgetnode(map, &code, 3, EnumFindBehaviorFuzzy);
    SP_EQUAL(node, map->root);
    code.code[0] = 'A';
    code.code[1] = 'A';
    code.code[2] = 'B';
    node = imapgetnode(map, &code, 3, EnumFindBehaviorFuzzy);
    SP_EQUAL(node, map->root->childs[0]->childs[0]);
    code.code[1] = 'D';
    node = imapgetnode(map, &code, 3, EnumFindBehaviorFuzzy);
    SP_EQUAL(node, map->root->childs[0]);
    
    _aoi_print(map, EnumNodePrintStateAll);
    
    imapremoveunit(map, __getunitfor(3));
    SP_EQUAL(map->state.nodecount, 3);
    SP_EQUAL(map->state.leafcount, 1);
    SP_EQUAL(map->state.unitcount, 3);
    
    SP_EQUAL(irefcachesize(map->nodecache), 1);
    
    imapremoveunit(map, __getunitfor(0));
    SP_EQUAL(map->state.nodecount, 3);
    SP_EQUAL(map->state.leafcount, 1);
    SP_EQUAL(map->state.unitcount, 2);
    
    imapremoveunit(map, __getunitfor(1));
    SP_EQUAL(map->state.nodecount, 3);
    SP_EQUAL(map->state.leafcount, 1);
    SP_EQUAL(map->state.unitcount, 1);
    
    imapremoveunit(map, __getunitfor(2));
    
    SP_EQUAL(map->state.nodecount, 0);
    SP_EQUAL(map->state.leafcount, 0);
    SP_EQUAL(map->state.unitcount, 0);
    
    SP_EQUAL(irefcachesize(map->nodecache), 4);
    
    SP_EQUAL(NULL, __getunitfor(0)->node);
    SP_EQUAL(NULL, __getunitfor(1)->node);
    SP_EQUAL(NULL, __getunitfor(2)->node);
    SP_EQUAL(NULL, __getunitfor(3)->node);
    
    __resetu(0);
    __resetu(1);
    __resetu(2);
    __resetu(3);
}


SP_CASE(imap, imapupdateunit) {
    
    __setu(0, 0.8, 0.8);
    
    SP_EQUAL(map->state.nodecount, 0);
    SP_EQUAL(map->state.leafcount, 0);
    SP_EQUAL(map->state.unitcount, 0);
    
    imapaddunit(map, __getunitfor(0));
    icode code = {.code={'A','A','A',0}};
    
    /**
     |BBB| BBD| BDB| BDD| DBB| DBD| DDB| DDD|
     |_______________________________________
     |BBA| BBC| BDA| BDC| DBA| DBC| DDA| DDC|
     |_______________________________________
     |BAB| BAD| BCB| BCD| DAB| DAD| DCB| DCD|
     |_______________________________________
     |BAA| BAC| BCA| BCC| DAA| DAC| DCA| DCC|
     |_______________________________________
     |ABB| ABD| ADB| ADD| CBB| CBD| CDB| CDD|
     |_______________________________________
     |ABA| ABC| ADA| ADC| CBA| CBC| CDA| CDC|
     |_______________________________________
     |AAB| AAD| ACB| ACD| CAB| CAD| CCB| CCD|
     |_______________________________________
     |AAA:[0]| AAC| ACA| ACC| CAA| CAC| CCA| CCC|
     |_______________________________________
     */
    
    SP_EQUAL(map->state.nodecount, 3);
    SP_EQUAL(map->state.leafcount, 1);
    SP_EQUAL(map->state.unitcount, 1);
    SP_EQUAL(imapgetnode(map, &code, 3, EnumFindBehaviorAccurate),
                     __getunitfor(0)->node);
    
    __setu(0, 0.1, 0.1);
    
    /**
     |BBB| BBD| BDB| BDD| DBB| DBD| DDB| DDD|
     |_______________________________________
     |BBA| BBC| BDA| BDC| DBA| DBC| DDA| DDC|
     |_______________________________________
     |BAB| BAD| BCB| BCD| DAB| DAD| DCB| DCD|
     |_______________________________________
     |BAA| BAC| BCA| BCC| DAA| DAC| DCA| DCC|
     |_______________________________________
     |ABB| ABD| ADB| ADD| CBB| CBD| CDB| CDD|
     |_______________________________________
     |ABA| ABC| ADA| ADC| CBA| CBC| CDA| CDC|
     |_______________________________________
     |AAB| AAD| ACB| ACD| CAB| CAD| CCB| CCD|
     |_______________________________________
     |AAA:[0]| AAC| ACA| ACC| CAA| CAC| CCA| CCC|
     |_______________________________________
     */
    imapupdateunit(map, __getunitfor(0));
    SP_EQUAL(map->state.nodecount, 3);
    SP_EQUAL(map->state.leafcount, 1);
    SP_EQUAL(map->state.unitcount, 1);
    SP_EQUAL(imapgetnode(map, &code, 3, EnumFindBehaviorAccurate),
                     __getunitfor(0)->node);
    
    
    /**
     |BBB| BBD| BDB| BDD| DBB| DBD| DDB| DDD|
     |_______________________________________
     |BBA| BBC| BDA| BDC| DBA| DBC| DDA| DDC|
     |_______________________________________
     |BAB| BAD| BCB| BCD| DAB| DAD| DCB| DCD|
     |_______________________________________
     |BAA| BAC| BCA| BCC| DAA| DAC| DCA| DCC|
     |_______________________________________
     |ABB| ABD| ADB| ADD| CBB| CBD| CDB| CDD|
     |_______________________________________
     |ABA| ABC| ADA| ADC| CBA| CBC| CDA| CDC|
     |_______________________________________
     |AAB| AAD| ACB| ACD| CAB| CAD| CCB| CCD|
     |_______________________________________
     |AAA:[0,1]| AAC| ACA| ACC| CAA| CAC| CCA| CCC|
     |_______________________________________
     */
    code.code[0] = 'A'; code.code[1] = 'A'; code.code[2] = 'A';
    int64_t ts = imapgetnode(map, &code, 0, EnumFindBehaviorAccurate)->tick;
    int64_t tsA = imapgetnode(map, &code, 1, EnumFindBehaviorAccurate)->tick;
    int64_t tsAA = imapgetnode(map, &code, 2, EnumFindBehaviorAccurate)->tick;
    int64_t tsAAA = imapgetnode(map, &code, 3, EnumFindBehaviorAccurate)->tick;
    
    __resetu(1);
    imapaddunit(map, __getunitfor(1));
    int64_t nts = imapgetnode(map, &code, 0, EnumFindBehaviorAccurate)->tick;
    int64_t ntsA = imapgetnode(map, &code, 1, EnumFindBehaviorAccurate)->tick;
    int64_t ntsAA = imapgetnode(map, &code, 2, EnumFindBehaviorAccurate)->tick;
    int64_t ntsAAA = imapgetnode(map, &code, 3, EnumFindBehaviorAccurate)->tick;
    
    SP_EQUAL(nts > ts, 1);
    SP_EQUAL(ntsA > tsA, 1);
    SP_EQUAL(ntsAA > tsAA, 1);
    SP_EQUAL(ntsAAA > tsAAA, 1);
    
    __setu(0, 1.0, 1.0);
    imapupdateunit(map, __getunitfor(0));
    
    code.code[0] = 'A';
    code.code[1] = 'A';
    code.code[2] = 'D';
    SP_EQUAL(imapgetnode(map, &code, 3, EnumFindBehaviorAccurate),
                     __getunitfor(0)->node);
    
    int64_t nnts = imapgetnode(map, &code, 0, EnumFindBehaviorAccurate)->tick;
    int64_t nntsA = imapgetnode(map, &code, 1, EnumFindBehaviorAccurate)->tick;
    int64_t nntsAA = imapgetnode(map, &code, 2, EnumFindBehaviorAccurate)->tick;
    int64_t nntsAAA = imapgetnode(map, &code, 3, EnumFindBehaviorAccurate)->tick;
    SP_EQUAL(nts, nnts);
    SP_EQUAL(ntsA, nntsA);
    SP_EQUAL(ntsAA, nntsAA);
    SP_EQUAL(ntsAAA != nntsAAA, 1);
    code.code[2] = 'D';
    int64_t nntsAAD = imapgetnode(map, &code, 3, EnumFindBehaviorAccurate)->tick;
    
    SP_EQUAL(nntsAAD >= ntsAAA, 1);
    
    SP_EQUAL(map->state.nodecount, 4);
    SP_EQUAL(map->state.leafcount, 2);
    SP_EQUAL(map->state.unitcount, 2);
    
    
    /**
     |BBB| BBD| BDB| BDD| DBB| DBD| DDB| DDD|
     |_______________________________________
     |BBA| BBC| BDA| BDC| DBA| DBC| DDA| DDC|
     |_______________________________________
     |BAB| BAD| BCB| BCD| DAB| DAD| DCB| DCD|
     |_______________________________________
     |BAA| BAC| BCA| BCC| DAA| DAC| DCA| DCC|
     |_______________________________________
     |ABB| ABD| ADB| ADD:[0]| CBB| CBD| CDB| CDD|
     |_______________________________________
     |ABA| ABC| ADA| ADC| CBA| CBC| CDA| CDC|
     |_______________________________________
     |AAB| AAD| ACB| ACD| CAB| CAD| CCB| CCD|
     |_______________________________________
     |AAA:[1]| AAC| ACA| ACC| CAA| CAC| CCA| CCC|
     |_______________________________________
     */
    __setu(0, 3.0, 3.0);
    imapupdateunit(map, __getunitfor(0));
    // AAD --> ADD
    code.code[0] = 'A';
    code.code[1] = 'A';
    code.code[2] = 'D';
    
    int64_t nnnts = imapgetnode(map, &code, 0, EnumFindBehaviorAccurate)->tick;
    int64_t nnntsA = imapgetnode(map, &code, 1, EnumFindBehaviorAccurate)->tick;
    int64_t nnntsAA = imapgetnode(map, &code, 2, EnumFindBehaviorAccurate)->tick;
    SP_EQUAL(imapgetnode(map, &code, 3, EnumFindBehaviorAccurate), NULL);
    
    code.code[0] = 'A';
    code.code[1] = 'D';
    code.code[2] = 'D';
    
    SP_EQUAL(imapgetnode(map, &code, 3, EnumFindBehaviorAccurate),
                     __getunitfor(0)->node);
    
    int64_t nnnnts = imapgetnode(map, &code, 0, EnumFindBehaviorAccurate)->tick;
    int64_t nnnntsA = imapgetnode(map, &code, 1, EnumFindBehaviorAccurate)->tick;
    int64_t nnnntsAD = imapgetnode(map, &code, 2, EnumFindBehaviorAccurate)->tick;
    int64_t nnnntsADD = imapgetnode(map, &code, 3, EnumFindBehaviorAccurate)->tick;
    
    SP_EQUAL(nnnnts, nnnts);
    SP_EQUAL(nnnntsA, nnntsA);
    SP_EQUAL(nnnntsAD > nnntsAA, 1);
    SP_EQUAL(nnnntsADD > nnntsAA, 1);
    
    imapremoveunit(map, __getunitfor(0));
    imapremoveunit(map, __getunitfor(1));
    
    SP_EQUAL(map->state.nodecount, 0);
    SP_EQUAL(map->state.leafcount, 0);
    SP_EQUAL(map->state.unitcount, 0);
}

SP_CASE(imap, imapmovecode) {
    ipos pos = {.x=0, .y=0};
    isize size = {.w=512, .h = 512};
    imap *xxmap = imapmake(&pos, &size, 24); 

    icode code;
    imapgencode(xxmap, &pos, &code);

    printf("code: %s, %f, %f\n", code.code, code.pos.x, code.pos.y);
    SP_EQUAL(strlen(code.code), 24);

    icode xcode;
    printf("map divide precisions: %f, %f \n", 
            xxmap->nodesizes[24].w, 
            xxmap->nodesizes[24].h);

    // pow(2, 24) Up
    printf("Move Up\n");
    for(int i=0; i<1024; ++i) {

        imapmovecode(xxmap, &code, EnumCodeMoveUp);

        pos.x = pos.x;
        pos.y += xxmap->nodesizes[24].h;
        imapgencode(xxmap, &pos, &xcode);
        printf("icode: %s, %f, %f\n", code.code, code.pos.x, code.pos.y);
        printf("xcode: %s, %f, %f\n", xcode.code, xcode.pos.x, xcode.pos.y);

        SP_EQUAL(code.pos.x, pos.x);
        SP_EQUAL(code.pos.y, pos.y);

        SP_TRUE(strcmp(xcode.code, code.code) == 0);
    }

    // Down
    printf("Move Down\n");
    for(int i=0; i<1024; ++i) {

        imapmovecode(xxmap, &code, EnumCodeMoveDown);

        pos.x = pos.x;
        pos.y -= xxmap->nodesizes[24].h;
        imapgencode(xxmap, &pos, &xcode);
        printf("icode: %s, %f, %f\n", code.code, code.pos.x, code.pos.y);
        printf("xcode: %s, %f, %f\n", xcode.code, xcode.pos.x, xcode.pos.y);

        SP_EQUAL(code.pos.x, pos.x);
        SP_EQUAL(code.pos.y, pos.y);

        SP_TRUE(strcmp(xcode.code, code.code) == 0);
    }

    // Right
    printf("Move Right\n");
    for(int i=0; i<1024; ++i) {

        imapmovecode(xxmap, &code, EnumCodeMoveRight);

        pos.x += xxmap->nodesizes[24].w;
        pos.y = pos.y;
        imapgencode(xxmap, &pos, &xcode);
        printf("icode: %s, %f, %f\n", code.code, code.pos.x, code.pos.y);
        printf("xcode: %s, %f, %f\n", xcode.code, xcode.pos.x, xcode.pos.y);

        SP_EQUAL(code.pos.x, pos.x);
        SP_EQUAL(code.pos.y, pos.y);

        SP_TRUE(strcmp(xcode.code, code.code) == 0);
    }

    // Left
    printf("Move Left\n");
    for(int i=0; i<1024; ++i) {

        imapmovecode(xxmap, &code, EnumCodeMoveLeft);

        pos.x -= xxmap->nodesizes[24].w;
        pos.y = pos.y;
        imapgencode(xxmap, &pos, &xcode);
        printf("icode: %s, %f, %f\n", code.code, code.pos.x, code.pos.y);
        printf("xcode: %s, %f, %f\n", xcode.code, xcode.pos.x, xcode.pos.y);

        SP_EQUAL(code.pos.x, pos.x);
        SP_EQUAL(code.pos.y, pos.y);

        SP_TRUE(strcmp(xcode.code, code.code) == 0);
    }

    // 随机左移
    for (int i=0; i<1024; ++i) {
        pos.x = rand() % 512;
        pos.y = rand() % 512;

        imapgencode(xxmap, &pos, &xcode);
        printf("xcode: %s, %f, %f\n", xcode.code, xcode.pos.x, xcode.pos.y);
        if ( imapmovecode(xxmap, &xcode, EnumCodeMoveLeft) > 0 ) {
            pos.x -= xxmap->nodesizes[xxmap->divide].w;

            SP_EQUAL(xcode.pos.x, pos.x);
            SP_EQUAL(xcode.pos.y, pos.y);

            imapgencode(xxmap, &pos, &code);
            printf("icode: %s, %f, %f\n", code.code, code.pos.x, code.pos.y);
            printf("xcode: %s, %f, %f\n", xcode.code, xcode.pos.x, xcode.pos.y);

            SP_TRUE(strcmp(xcode.code, code.code) == 0);
        }
    }

    // 随机右移
    for (int i=0; i<1024; ++i) {
        pos.x = rand() % 512;
        pos.y = rand() % 512;

        imapgencode(xxmap, &pos, &xcode);
        if ( imapmovecode(xxmap, &xcode, EnumCodeMoveRight) > 0 ) {
            pos.x += xxmap->nodesizes[xxmap->divide].w;

            SP_EQUAL(xcode.pos.x, pos.x);
            SP_EQUAL(xcode.pos.y, pos.y);

            imapgencode(xxmap, &pos, &code);
            printf("icode: %s, %f, %f\n", code.code, code.pos.x, code.pos.y);
            printf("xcode: %s, %f, %f\n", xcode.code, xcode.pos.x, xcode.pos.y);

            SP_TRUE(strcmp(xcode.code, code.code) == 0);
        }
    }

    // 随机上移
    for (int i=0; i<1024; ++i) {
        pos.x = rand() % 512;
        pos.y = rand() % 512;

        imapgencode(xxmap, &pos, &xcode);
        if ( imapmovecode(xxmap, &xcode, EnumCodeMoveUp) > 0 ) {
            pos.y += xxmap->nodesizes[xxmap->divide].h;

            SP_EQUAL(xcode.pos.x, pos.x);
            SP_EQUAL(xcode.pos.y, pos.y);

            imapgencode(xxmap, &pos, &code);
            printf("icode: %s, %f, %f\n", code.code, code.pos.x, code.pos.y);
            printf("xcode: %s, %f, %f\n", xcode.code, xcode.pos.x, xcode.pos.y);

            SP_TRUE(strcmp(xcode.code, code.code) == 0);
        }
    }

    // 随机下移
    for (int i=0; i<1024; ++i) {
        pos.x = rand() % 512;
        pos.y = rand() % 512;

        imapgencode(xxmap, &pos, &xcode);
        if ( imapmovecode(xxmap, &xcode, EnumCodeMoveDown) > 0 ) {
            pos.y -= xxmap->nodesizes[xxmap->divide].h;

            SP_EQUAL(xcode.pos.x, pos.x);
            SP_EQUAL(xcode.pos.y, pos.y);

            imapgencode(xxmap, &pos, &code);
            printf("icode: %s, %f, %f\n", code.code, code.pos.x, code.pos.y);
            printf("xcode: %s, %f, %f\n", xcode.code, xcode.pos.x, xcode.pos.y);

            SP_TRUE(strcmp(xcode.code, code.code) == 0);
        }
    }

    imapfree(xxmap);
}

SP_CASE(imap, imapmovecodeedge) {
    ipos p = {.x=0, .y=0};
    isize s = {.w=512, .h=512};

    imap *xxmap = imapmake(&p, &s, 10);

    icode code;
    icode xcode;

    imapgencode(xxmap, &p, &code);
    imapgencode(xxmap, &p, &xcode);

    SP_TRUE(strcmp(code.code, xcode.code) == 0);

    // Can not move left
    { 
    int move = imapmovecode(xxmap, &code, EnumCodeMoveLeft);
    SP_EQUAL(move, 0);
    SP_TRUE(strcmp(code.code, xcode.code) == 0);
    SP_EQUAL(code.pos.x, p.x);
    SP_EQUAL(code.pos.y, p.y);
    }
    // Can not move down
    {
    int move = imapmovecode(xxmap, &code, EnumCodeMoveDown);
    SP_EQUAL(move, 0);
    SP_TRUE(strcmp(code.code, xcode.code) == 0);
    SP_EQUAL(code.pos.x, p.x);
    SP_EQUAL(code.pos.y, p.y);
    }
    // Can Move right
    {
    int move = imapmovecode(xxmap, &code, EnumCodeMoveRight);
    SP_EQUAL(move, 1);
    p.x += xxmap->nodesizes[xxmap->divide].w;
    imapgencode(xxmap, &p, &xcode);
    SP_TRUE(strcmp(code.code, xcode.code) == 0);
    SP_EQUAL(code.pos.x, p.x);
    SP_EQUAL(code.pos.y, p.y);
    }
    // Can Move up
    {
    p.x = 0; p.y=0;
    imapgencode(xxmap, &p, &code);
    int move = imapmovecode(xxmap, &code, EnumCodeMoveUp);
    SP_EQUAL(move, 1);
    p.y += xxmap->nodesizes[xxmap->divide].h;
    imapgencode(xxmap, &p, &xcode);
    SP_TRUE(strcmp(code.code, xcode.code) == 0);
    SP_EQUAL(code.pos.x, p.x);
    SP_EQUAL(code.pos.y, p.y);
    }

    imapfree(xxmap);

    SP_TRUE(1)
}

SP_CASE(imap, end) {
    SP_TRUE(1);
}

// **********************************************************************************
// ifilter
SP_SUIT(ifilter);

SP_CASE(ifilter, ifiltermake) {
    make_test_map();
    
    ifilter *filter = ifiltermake();
    
    SP_EQUAL(ifilterchecksum(map, filter), 0);
    
    ifilterfree(filter);
}

SP_CASE(ifilter, ifiltermake_circle) {
    
    ipos pos = {.x=0, .y=0};
    ifilter *filterrange = ifiltermake_circle(&pos, 2.0);
    
    SP_EQUAL(ifilterchecksum(map, filterrange) != 0, 1);
    
    SP_EQUAL(filterrange->circle.radis, 2.0);
    
    SP_EQUAL(filterrange->circle.pos.x, 0);
    SP_EQUAL(filterrange->circle.pos.y, 0);
    
    ifilterfree(filterrange);
}

SP_CASE(ifilter, ifiltermake_rect) {
    ipos pos = {.x=0, .y=0};
    isize size = {.w=2, .h=2};
    ifilter *filterrect = ifiltermake_rect(&pos, &size);
    
    SP_EQUAL(ifilterchecksum(map, filterrect) != 0, 1);
    SP_EQUAL(filterrect->rect.pos.x, 0);
    SP_EQUAL(filterrect->rect.pos.y, 0);
    SP_EQUAL(filterrect->rect.size.w, 2);
    SP_EQUAL(filterrect->rect.size.h, 2);
    
    ifilterfree(filterrect);
}

SP_CASE(ifilter, ifilteradd) {
    ifilter *filter = ifiltermake();
    
    ipos pos = {.x=0, .y=0};
    ifilter *filterrange = ifiltermake_circle(&pos, 2.0);
    
    SP_EQUAL(ifilterchecksum(map, filter), 0);
    
    ifilteradd(filter, filterrange);
    
    SP_EQUAL(ifilterchecksum(map, filter) != 0, 1);
    
    ifilterfree(filter);
    ifilterfree(filterrange);
}

SP_CASE(ifilter, ifilterremove) {
    ifilter *filter = ifiltermake();
    
    ipos pos = {.x=0, .y=0};
    ifilter *filterrange = ifiltermake_circle(&pos, 2.0);
    
    SP_EQUAL(ifilterchecksum(map, filter), 0);
    
    SP_EQUAL(ireflistlen(filter->list), 0);
    
    ifilteradd(filter, filterrange);
    
    SP_EQUAL(ifilterchecksum(map, filter) != 0, 1);
    
    SP_EQUAL(ireflistlen(filter->list), 1);
    
    ifilterremove(filter, filterrange);
    
    SP_EQUAL(ireflistlen(filter->list), 0);
    
    ifilterfree(filter);
    ifilterfree(filterrange);
}

SP_CASE(ifilter, ifilterclean) {
    ifilter *filter = ifiltermake();
    
    ipos pos = {.x=0, .y=0};
    ifilter *filterrange = ifiltermake_circle(&pos, 2.0);
    
    SP_EQUAL(ifilterchecksum(map, filter), 0);
    
    SP_EQUAL(ireflistlen(filter->list), 0);
    
    ifilteradd(filter, filterrange);
    ifilteradd(filter, filterrange);
    
    SP_EQUAL(ifilterchecksum(map, filter) != 0, 1);
    
    SP_EQUAL(ireflistlen(filter->list), 2);
    
    ifilterclean(filter);
    
    SP_EQUAL(ireflistlen(filter->list), 0);
    
    ifilterfree(filter);
    ifilterfree(filterrange);
}

int __filter_test_forid(imap *map, ifilter *filter, iunit *unit) {
    icheckret(filter, iino);
    icheckret(unit, iino);
    
    if (unit->id == filter->id) {
        return iiok;
    }
    
    return iino;
}

SP_CASE(ifilter, ifilterrun) {
    
    // prepare units in map
    __setu(0, 0.1, 0.1);
    
    imapaddunit(map, __getunitfor(0));
    
    __setu(1, 0.3, 0.3);
    
    imapaddunit(map, __getunitfor(1));
    
    __setu(2, 1.0, 1.0);
    
    imapaddunit(map, __getunitfor(2));
    
    /**
     |BBB| BBD| BDB| BDD| DBB| DBD| DDB| DDD|
     |_______________________________________
     |BBA| BBC| BDA| BDC| DBA| DBC| DDA| DDC|
     |_______________________________________
     |BAB| BAD| BCB| BCD| DAB| DAD| DCB| DCD|
     |_______________________________________
     |BAA| BAC| BCA| BCC| DAA| DAC| DCA| DCC|
     |_______________________________________
     |ABB| ABD| ADB| ADD:[0]| CBB| CBD| CDB| CDD|
     |_______________________________________
     |ABA| ABC| ADA| ADC| CBA| CBC| CDA| CDC|
     |_______________________________________
     |AAB| AAD:[2]| ACB| ACD| CAB| CAD| CCB| CCD|
     |_______________________________________
     |AAA:[0,1]| AAC| ACA| ACC| CAA| CAC| CCA| CCC|
     |_______________________________________
     */
 
    
    ifilter *filter = ifiltermake();
    
    int ok ;
    ok = ifilterrun(map, filter, __getunitfor(0));
    SP_EQUAL(ok, iiok);
    ok = ifilterrun(map, filter, __getunitfor(1));
    SP_EQUAL(ok, iiok);
    ok = ifilterrun(map, filter, __getunitfor(2));
    SP_EQUAL(ok, iiok);
    
    ipos pos = {.x=0, .y=0};
    ifilter *filterrange = ifiltermake_circle(&pos, 1.0);
    
    SP_EQUAL(ifilterchecksum(map, filter), 0);
    
    SP_EQUAL(ireflistlen(filter->list), 0);
    
    ifilteradd(filter, filterrange);
    
    SP_EQUAL(ifilterchecksum(map, filter) != 0, 1);
    
    ok = ifilterrun(map, filter, __getunitfor(0));
    SP_EQUAL(ok, iiok);
    ok = ifilterrun(map, filter, __getunitfor(1));
    SP_EQUAL(ok, iiok);
    ok = ifilterrun(map, filter, __getunitfor(2));
    SP_EQUAL(ok, iino);
    
    __setu(0, -0.3, -0.4);
    imapupdateunit(map, __getunitfor(0));
    SP_EQUAL(__getunitfor(0)->node->code.code[2], 'A');
    ok = ifilterrun(map, filter, __getunitfor(0));
    SP_EQUAL(ok, iiok);
    
    __setu(0, 1.3, 1.4);
    imapupdateunit(map, __getunitfor(0));
    SP_EQUAL(__getunitfor(0)->node->code.code[2], 'D');
    ok = ifilterrun(map, filter, __getunitfor(0));
    SP_EQUAL(ok, iino);
    
    // 过滤ID
    ok = ifilterrun(map, filter, __getunitfor(1));
    SP_EQUAL(ok, iiok);
    filter->id = 2;
    filter->entry = __filter_test_forid;
    ok = ifilterrun(map, filter, __getunitfor(1));
    SP_EQUAL(ok, iino);
    
    // 清理 map
    imapremoveunit(map, __getunitfor(0));
    imapremoveunit(map, __getunitfor(1));
    imapremoveunit(map, __getunitfor(2));
    
    ifilterfree(filter);
    ifilterfree(filterrange);
}

SP_CASE(ifilter, imapcollectunit) {
    
    // prepare units in map
    __setu(0, 0.1, 0.0);
    
    imapaddunit(map, __getunitfor(0));
    
    __setu(1, 0.3, 0.4);
    
    imapaddunit(map, __getunitfor(1));
    
    __setu(2, 0.5, 0.0);
    
    imapaddunit(map, __getunitfor(2));
    
    __setu(3, 1.5, 1.2);
    
    imapaddunit(map, __getunitfor(3));
    
    __setu(4, 1.5, 0.2);
    
    imapaddunit(map, __getunitfor(4));
    
    __setu(5, 1.0, 0.0);
    
    imapaddunit(map, __getunitfor(5));
    
    /**
     |BBB| BBD| BDB| BDD| DBB| DBD| DDB| DDD|
     |_______________________________________
     |BBA| BBC| BDA| BDC| DBA| DBC| DDA| DDC|
     |_______________________________________
     |BAB| BAD| BCB| BCD| DAB| DAD| DCB| DCD|
     |_______________________________________
     |BAA| BAC| BCA| BCC| DAA| DAC| DCA| DCC|
     |_______________________________________
     |ABB| ABD| ADB| ADD:[0]| CBB| CBD| CDB| CDD|
     |_______________________________________
     |ABA| ABC| ADA| ADC| CBA| CBC| CDA| CDC|
     |_______________________________________
     |AAB| AAD:[3,5]| ACB| ACD| CAB| CAD| CCB| CCD|
     |_______________________________________
     |AAA:[0,1,2]| AAC:[4]| ACA| ACC| CAA| CAC| CCA| CCC|
     |_______________________________________
     */
    
    
    ifilter *filter = ifiltermake();
    
    ipos pos = {.x=0, .y=0};
    ifilter *filterrange = ifiltermake_circle(&pos, 1.0);
    
    SP_EQUAL(ifilterchecksum(map, filter), 0);
    
    SP_EQUAL(ireflistlen(filter->list), 0);
    
    ifilteradd(filter, filterrange);
    
    SP_EQUAL(ifilterchecksum(map, filter) != 0, 1);
    
    ireflist *list = ireflistmake();
    
    icode code = {.code={'A', 'A', 0}};
    
    inode *node = imapgetnode(map, &code, 2, EnumFindBehaviorAccurate);

#define __range(x) (x)
    // ------------------------------------------------------------------------------------
    // (0: 0.1, 0.0) (1: 0.3, 0.4) (2: 0.5, 0.0) (3: 1.5, 1.2) (4: 1.5, 0.2) (5: 1.0, 1.0)
    // ------------------------------------------------------------------------------------
    ireflist *snap = ireflistmake();
    filterrange->circle.radis = __range(2.0);
    // (0: 0.1, 0.0) (1: 0.3, 0.4) (2: 0.5, 0.0) (3: 1.5, 1.2) (4: 1.5, 0.2) (5: 1.0, 1.0)
    imapcollectunit(map, node, list, filter, snap);
    imapcollectcleanunittag(map, snap);
    SP_EQUAL(ireflistlen(list), 6);
    
    ireflistremoveall(list);
    filterrange->circle.radis = __range(0.1);
    // (0: 0.1, 0.0)
    imapcollectunit(map, node, list, filter, snap);
    imapcollectcleanunittag(map, snap);
    SP_EQUAL(ireflistlen(list), 1);
    SP_EQUAL(ireflistfind(list, irefcast(__getunitfor(0))) != NULL, 1);
    SP_EQUAL(icast(iunit, ireflistfirst(list)->value)->id, 0);
    
    ireflistremoveall(list);
    filterrange->circle.radis = __range(0.5);
    // (0: 0.1, 0.0) (1: 0.3, 0.4) (2: 0.5, 0.0)
    imapcollectunit(map, node, list, filter, snap);
    imapcollectcleanunittag(map, snap);
    SP_EQUAL(ireflistlen(list), 3);
    SP_EQUAL(ireflistfind(list, irefcast(__getunitfor(0))) != NULL, 1);
    SP_EQUAL(ireflistfind(list, irefcast(__getunitfor(1))) != NULL, 1);
    SP_EQUAL(ireflistfind(list, irefcast(__getunitfor(2))) != NULL, 1);
    
    ireflistremoveall(list);
    filterrange->circle.radis = __range(1.5);
    // (0: 0.1, 0.0) (1: 0.3, 0.4) (2: 0.5, 0.0) (5: 1.0, 1.0)
    imapcollectunit(map, node, list, filter, snap);
    imapcollectcleanunittag(map, snap);
    SP_EQUAL(ireflistlen(list), 4);
    SP_EQUAL(ireflistfind(list, irefcast(__getunitfor(0))) != NULL, 1);
    SP_EQUAL(ireflistfind(list, irefcast(__getunitfor(1))) != NULL, 1);
    SP_EQUAL(ireflistfind(list, irefcast(__getunitfor(2))) != NULL, 1);
    SP_EQUAL(ireflistfind(list, irefcast(__getunitfor(5))) != NULL, 1);
    
    ireflistremoveall(list);
    filterrange->circle.radis = __range(1.7);
    // (0: 0.1, 0.0) (1: 0.3, 0.4) (2: 0.5, 0.0) (4: 1.5, 0.2) (5: 1.0, 1.0)
    imapcollectunit(map, node, list, filter, snap);
    imapcollectcleanunittag(map, snap);
    SP_EQUAL(ireflistlen(list), 5);
    SP_EQUAL(ireflistfind(list, irefcast(__getunitfor(0))) != NULL, 1);
    SP_EQUAL(ireflistfind(list, irefcast(__getunitfor(1))) != NULL, 1);
    SP_EQUAL(ireflistfind(list, irefcast(__getunitfor(2))) != NULL, 1);
    SP_EQUAL(ireflistfind(list, irefcast(__getunitfor(4))) != NULL, 1);
    SP_EQUAL(ireflistfind(list, irefcast(__getunitfor(5))) != NULL, 1);
    
    // 清理 map
    imapremoveunit(map, __getunitfor(0));
    imapremoveunit(map, __getunitfor(1));
    imapremoveunit(map, __getunitfor(2));
    imapremoveunit(map, __getunitfor(3));
    imapremoveunit(map, __getunitfor(4));
    imapremoveunit(map, __getunitfor(5));
    
    ifilterfree(filter);
    ifilterfree(filterrange);
    ireflistfree(list);
    ireflistfree(snap);
}

SP_CASE(ifilter, end) {
    SP_TRUE(1);
}


// **********************************************************************************
// isearchresult
SP_SUIT(isearchresult);

SP_CASE(isearchresult, isearchresultmake) {
    isearchresult *search = isearchresultmake();
    
    SP_EQUAL(search->filter != NULL, 1);
    SP_EQUAL(search->units != NULL, 1);
    
    isearchresultfree(search);
}

SP_CASE(isearchresult, isearchresultattach) {
    isearchresult *search = isearchresultmake();
    
    SP_EQUAL(search->filter != NULL, 1);
    SP_EQUAL(search->units != NULL, 1);
    
    ifilter *filter = ifiltermake();
    isearchresultattach(search, filter);
    SP_EQUAL(search->filter, filter);
    
    isearchresultfree(search);
    ifilterfree(filter);
}

SP_CASE(isearchresult, isearchresultdettach) {
    isearchresult *search = isearchresultmake();
    
    SP_EQUAL(search->filter != NULL, 1);
    SP_EQUAL(search->units != NULL, 1);
    
    ifilter *filter = ifiltermake();
    isearchresultattach(search, filter);
    SP_EQUAL(search->filter, filter);
    
    isearchresultdettach(search);
    
    SP_EQUAL(search->filter, NULL);
    
    isearchresultfree(search);
    ifilterfree(filter);
}

// **********************************************************************************
SP_SUIT(searching);

SP_CASE(searching, imapsearchfromposANDimapsearchfromnode) {
    // prepare units in map
    __setu(0, 0.1, 0.0);
    
    imapaddunit(map, __getunitfor(0));
    
    __setu(1, 0.3, 0.4);
    
    imapaddunit(map, __getunitfor(1));
    
    __setu(2, 0.5, 0.0);
    
    imapaddunit(map, __getunitfor(2));
    
    __setu(3, 1.5, 1.2);
    
    imapaddunit(map, __getunitfor(3));
    
    __setu(4, 1.5, 0.2);
    
    imapaddunit(map, __getunitfor(4));
    
    __setu(5, 1.0, 0.0);
    
    imapaddunit(map, __getunitfor(5));
    
    /**
     |BBB| BBD| BDB| BDD| DBB| DBD| DDB| DDD|
     |_______________________________________
     |BBA| BBC| BDA| BDC| DBA| DBC| DDA| DDC|
     |_______________________________________
     |BAB| BAD| BCB| BCD| DAB| DAD| DCB| DCD|
     |_______________________________________
     |BAA| BAC| BCA| BCC| DAA| DAC| DCA| DCC|
     |_______________________________________
     |ABB| ABD| ADB| ADD| CBB| CBD| CDB| CDD|
     |_______________________________________
     |ABA| ABC| ADA| ADC| CBA| CBC| CDA| CDC|
     |_______________________________________
     |AAB| AAD:[3,5]| ACB| ACD| CAB| CAD| CCB| CCD|
     |_______________________________________
     |AAA:[0,1,2]| AAC:[4]| ACA| ACC| CAA| CAC| CCA| CCC|
     |_______________________________________
     */
    
    isearchresult *result = isearchresultmake();
    
    SP_EQUAL(result->tick, 0);
    SP_EQUAL(result->checksum, 0);
    
    ipos pos = {.x=0,.y=0};
    icode code = {.code={'A','A','A',0}};
    imapsearchfrompos(map, &pos, result, 0.1);
    int64_t ts = result->tick;
    int64_t checksum = result->checksum;
    inode *node = imapgetnode(map, &code, 3, EnumFindBehaviorAccurate);
    SP_EQUAL(ts, node->tick);
    SP_EQUAL(ireflistlen(result->units), 1);
    SP_EQUAL(checksum != 0, 1);
    
    imapsearchfrompos(map, &pos, result, 0.1);
    ts = result->tick;
    SP_EQUAL(ts, node->tick);
    SP_EQUAL(ireflistlen(result->units), 1);
    SP_EQUAL(checksum, result->checksum);
    
    // add unit 6
    __setu(6, 0.05, 0.05);
    imapaddunit(map, __getunitfor(6));
    /**
     |BBB| BBD| BDB| BDD| DBB| DBD| DDB| DDD|
     |_______________________________________
     |BBA| BBC| BDA| BDC| DBA| DBC| DDA| DDC|
     |_______________________________________
     |BAB| BAD| BCB| BCD| DAB| DAD| DCB| DCD|
     |_______________________________________
     |BAA| BAC| BCA| BCC| DAA| DAC| DCA| DCC|
     |_______________________________________
     |ABB| ABD| ADB| ADD| CBB| CBD| CDB| CDD|
     |_______________________________________
     |ABA| ABC| ADA| ADC| CBA| CBC| CDA| CDC|
     |_______________________________________
     |AAB| AAD:[3,5]| ACB| ACD| CAB| CAD| CCB| CCD|
     |_______________________________________
     |AAA:[0,1,2,6]| AAC:[4]| ACA| ACC| CAA| CAC| CCA| CCC|
     |_______________________________________
     */
    imapsearchfrompos(map, &pos, result, 0.1);
    int64_t nts = result->tick;
    SP_EQUAL(nts, node->tick);
    SP_EQUAL(nts > ts, 1);
    SP_EQUAL(ireflistlen(result->units), 2);
    SP_EQUAL(checksum != result->checksum, 1);
    checksum = result->checksum;
    
    // ------------------------------------------------------------------------------------
    // (0: 0.1, 0.0)    (1: 0.3, 0.4)   (2: 0.5, 0.0)   (3: 1.5, 1.2)   (4: 1.5, 0.2)
    // (5: 1.0, 1.0)    (6: 0.05, 0.05)
    // ------------------------------------------------------------------------------------
    imapsearchfrompos(map, &pos, result, 0.5);
    int64_t nnts = result->tick;
    SP_EQUAL(result->tick, node->tick);
    SP_EQUAL(nnts == nts, 1);
    SP_EQUAL(ireflistlen(result->units), 4);
    SP_EQUAL(checksum != result->checksum, 1);
    
    
    __setu(7, 3.99, 3.99);
    imapaddunit(map, __getunitfor(7));
    /**
     |BBB| BBD| BDB| BDD| DBB| DBD| DDB| DDD|
     |_______________________________________
     |BBA| BBC| BDA| BDC| DBA| DBC| DDA| DDC|
     |_______________________________________
     |BAB| BAD| BCB| BCD| DAB| DAD| DCB| DCD|
     |_______________________________________
     |BAA| BAC| BCA| BCC| DAA| DAC| DCA| DCC|
     |_______________________________________
     |ABB| ABD| ADB| ADD:[7]| CBB| CBD| CDB| CDD|
     |_______________________________________
     |ABA| ABC| ADA| ADC| CBA| CBC| CDA| CDC|
     |_______________________________________
     |AAB| AAD:[3,5]| ACB| ACD| CAB| CAD| CCB| CCD|
     |_______________________________________
     |AAA:[0,1,2,6]| AAC:[4]| ACA| ACC| CAA| CAC| CCA| CCC|
     |_______________________________________
     */
    pos.x = 4;
    pos.y = 4;
    // ------------------------------------------------------------------------------------
    // (0: 0.1, 0.0)    (1: 0.3, 0.4)   (2: 0.5, 0.0)   (3: 1.5, 1.2)   (4: 1.5, 0.2)
    // (5: 1.0, 1.0)    (6: 0.05, 0.05) (7:3.99, 3.99)
    // ------------------------------------------------------------------------------------
    imapsearchfrompos(map, &pos, result, 0.5);
    int64_t nnnts = result->tick;
    __setcode(code, 'A', 'D', 'D');
    node = __getnode(code, 3);
    SP_EQUAL(result->tick, node->tick);
    SP_EQUAL(nnnts == nnts, 0);
    SP_EQUAL(ireflistlen(result->units), 1);
    SP_EQUAL(checksum != result->checksum, 1);
    
    __setu(8, 4.01, 4.01);
    imapaddunit(map, __getunitfor(8));
    /**
     |BBB| BBD| BDB| BDD| DBB| DBD| DDB| DDD|
     |_______________________________________
     |BBA| BBC| BDA| BDC| DBA| DBC| DDA| DDC|
     |_______________________________________
     |BAB| BAD| BCB| BCD| DAB| DAD| DCB| DCD|
     |_______________________________________
     |BAA| BAC| BCA| BCC| DAA:[8]| DAC| DCA| DCC|
     |_______________________________________
     |ABB| ABD| ADB| ADD:[7]| CBB| CBD| CDB| CDD|
     |_______________________________________
     |ABA| ABC| ADA| ADC| CBA| CBC| CDA| CDC|
     |_______________________________________
     |AAB| AAD:[3,5]| ACB| ACD| CAB| CAD| CCB| CCD|
     |_______________________________________
     |AAA:[0,1,2,6]| AAC:[4]| ACA| ACC| CAA| CAC| CCA| CCC|
     |_______________________________________
     */
    // ------------------------------------------------------------------------------------
    // (0: 0.1, 0.0)    (1: 0.3, 0.4)   (2: 0.5, 0.0)   (3: 1.5, 1.2)   (4: 1.5, 0.2)
    // (5: 1.0, 1.0)    (6: 0.05, 0.05) (7:3.99, 3.99)  (8: 4.01, 4.01)
    // ------------------------------------------------------------------------------------
    imapsearchfrompos(map, &pos, result, 0.5);
    int64_t nnnnts = result->tick;
    __setcode(code, 'A', 'D', 'D');
    node = __getnode(code, 0);
    SP_EQUAL(result->tick, node->tick);
    SP_EQUAL(nnnnts == nnnts, 0);
    SP_EQUAL(ireflistlen(result->units), 2);
    SP_EQUAL(checksum != result->checksum, 1);
    
    __setu(8, 5.0, 5.0);
    imapupdateunit(map, __getunitfor(8));
    /**
     |BBB| BBD| BDB| BDD| DBB| DBD| DDB| DDD|
     |_______________________________________
     |BBA| BBC| BDA| BDC| DBA| DBC| DDA| DDC|
     |_______________________________________
     |BAB| BAD| BCB| BCD| DAB| DAD:[8]| DCB| DCD|
     |_______________________________________
     |BAA| BAC| BCA| BCC| DAA| DAC| DCA| DCC|
     |_______________________________________
     |ABB| ABD| ADB| ADD:[7]| CBB| CBD| CDB| CDD|
     |_______________________________________
     |ABA| ABC| ADA| ADC| CBA| CBC| CDA| CDC|
     |_______________________________________
     |AAB| AAD:[3,5]| ACB| ACD| CAB| CAD| CCB| CCD|
     |_______________________________________
     |AAA:[0,1,2,6]| AAC:[4]| ACA| ACC| CAA| CAC| CCA| CCC|
     |_______________________________________
     */
    // ------------------------------------------------------------------------------------
    // (0: 0.1, 0.0)    (1: 0.3, 0.4)   (2: 0.5, 0.0)   (3: 1.5, 1.2)   (4: 1.5, 0.2)
    // (5: 1.0, 1.0)    (6: 0.05, 0.05) (7:3.99, 3.99)  (8: 5.0, 5.0)
    // ------------------------------------------------------------------------------------
    imapsearchfrompos(map, &pos, result, 0.5);
    int64_t nnnnnts = result->tick;
    __setcode(code, 'A', 'D', 'D');
    node = __getnode(code, 3);
    SP_EQUAL(result->tick, node->tick);
    SP_EQUAL(nnnnnts == nnnnts, 0);
    SP_EQUAL(ireflistlen(result->units), 1);
    SP_EQUAL(checksum != result->checksum, 1);
    
    isearchresultfree(result);
    
    // remove all unit
    for (int i=0; i<=20; ++i) {
        imapremoveunit(map, __getunitfor(i));
    }
}

SP_CASE(searching, imapsearchfromunit) {
    // prepare units in map
    __setu(0, 0.1, 0.0);
    
    imapaddunit(map, __getunitfor(0));
    
    __setu(1, 0.3, 0.4);
    
    imapaddunit(map, __getunitfor(1));
    
    __setu(2, 0.5, 0.0);
    
    imapaddunit(map, __getunitfor(2));
    
    __setu(3, 1.5, 1.2);
    
    imapaddunit(map, __getunitfor(3));
    
    __setu(4, 1.5, 0.2);
    
    imapaddunit(map, __getunitfor(4));
    
    __setu(5, 1.0, 0.0);
    
    imapaddunit(map, __getunitfor(5));
    
    /**
     |BBB| BBD| BDB| BDD| DBB| DBD| DDB| DDD|
     |_______________________________________
     |BBA| BBC| BDA| BDC| DBA| DBC| DDA| DDC|
     |_______________________________________
     |BAB| BAD| BCB| BCD| DAB| DAD| DCB| DCD|
     |_______________________________________
     |BAA| BAC| BCA| BCC| DAA| DAC| DCA| DCC|
     |_______________________________________
     |ABB| ABD| ADB| ADD| CBB| CBD| CDB| CDD|
     |_______________________________________
     |ABA| ABC| ADA| ADC| CBA| CBC| CDA| CDC|
     |_______________________________________
     |AAB| AAD:[3]| ACB| ACD| CAB| CAD| CCB| CCD|
     |_______________________________________
     |AAA:[0,1,2]| AAC:[4,5]| ACA| ACC| CAA| CAC| CCA| CCC|
     |_______________________________________
     */
    
    isearchresult *result = isearchresultmake();
    
    SP_EQUAL(result->tick, 0);
    SP_EQUAL(result->checksum, 0);
    
    // ------------------------------------------------------------------------------------
    // (0: 0.1, 0.0)    (1: 0.3, 0.4)   (2: 0.5, 0.0)   (3: 1.5, 1.2)   (4: 1.5, 0.2)
    // (5: 1.0, 0.0)
    // ------------------------------------------------------------------------------------
    icode code = {.code={'A','A','A',0}};
    imapsearchfromunit(map, __getunitfor(0), result, 0.45);
    int64_t ts = result->tick;
    int64_t checksum = result->checksum;
    inode *node = imapgetnode(map, &code, 3, EnumFindBehaviorAccurate);
    SP_EQUAL(ts, node->tick);
    SP_EQUAL(ireflistlen(result->units), 2);
    SP_EQUAL(checksum != 0, 1);
    
    imapsearchfromunit(map, __getunitfor(0), result, 0.45);
    ts = result->tick;
    SP_EQUAL(ts, node->tick);
    SP_EQUAL(ireflistlen(result->units), 2);
    SP_EQUAL(checksum, result->checksum);
    
    isearchresultfree(result);
    
    _aoi_print(map, EnumNodePrintStateNode);
    
    __setu(5, 1.0, 0.3);
    imapupdateunit(map, __getunitfor(5));
    
    _aoi_print(map, EnumNodePrintStateNode);
    
    __setu(5, 1.0, -2.0);
    imapupdateunit(map, __getunitfor(5));
    _aoi_print(map, EnumNodePrintStateNode);
    
    // remove all unit
    for (int i=0; i<=20; ++i) {
        imapremoveunit(map, __getunitfor(i));
    }
}

SP_CASE(searching, utick) {
    /**
     |BBB| BBD| BDB| BDD| DBB| DBD| DDB| DDD|
     |_______________________________________
     |BBA| BBC| BDA| BDC| DBA| DBC| DDA| DDC|
     |_______________________________________
     |BAB| BAD| BCB| BCD| DAB| DAD:[5]| DCB| DCD|
     |_______________________________________
     |BAA| BAC| BCA| BCC| DAA:[4]| DAC| DCA| DCC|
     |_______________________________________
     |ABB| ABD| ADB| ADD:[3]| CBB| CBD| CDB| CDD|
     |_______________________________________
     |ABA| ABC| ADA:[2]| ADC| CBA| CBC| CDA| CDC|
     |_______________________________________
     |AAB| AAD:[1]| ACB| ACD| CAB| CAD| CCB| CCD|
     |_______________________________________
     |AAA:[0]| AAC| ACA| ACC| CAA| CAC| CCA| CCC|
     |_______________________________________
     */
    __setu(0, 0, 0);
    imapaddunit(map, __getunitfor(0));
    
    __setu(1, 1.0, 1.0);
    imapaddunit(map, __getunitfor(1));
    
    __setu(2, 2, 2);
    imapaddunit(map, __getunitfor(2));
    
    __setu(3, 3, 3);
    imapaddunit(map, __getunitfor(3));
    
    __setu(4, 4, 4);
    imapaddunit(map, __getunitfor(4));
    
    __setu(5, 5, 5);
    
    _aoi_print(map, EnumNodePrintStateNode);
    
    isearchresult *result = isearchresultmake();
    
    imapsearchfromunit(map, __getunitfor(0), result, 0.4);
    
    SP_EQUAL(result->tick, __getunitfor(0)->tick);
    SP_EQUAL(result->tick, __getnode(__getunitfor(0)->code, 3)->tick);
    SP_EQUAL(ireflistlen(result->units), 0);
    
    __setu(1, 1.1, 1.1);
    imapupdateunit(map, __getunitfor(1));
    _aoi_print(map, EnumNodePrintStateNode);
    
    // AAA ->tick
    imapsearchfromunit(map, __getunitfor(0), result, 0.4);
    SP_EQUAL(result->tick, __getunitfor(0)->tick);
    SP_EQUAL(result->tick, __getnode(__getunitfor(0)->code, 3)->tick);
    SP_EQUAL(ireflistlen(result->units), 0);
    
    // AA ->utick
    imapsearchfromunit(map, __getunitfor(0), result, 0.8);
    SP_EQUAL(result->tick, __getnode(__getunitfor(0)->code, 2)->utick);
    SP_TRUE(result->tick != __getnode(__getunitfor(0)->code, 2)->tick);
    SP_EQUAL(ireflistlen(result->units), 0);
    SP_TRUE(__getnode(__getunitfor(0)->code, 2)->utick == __getnode(__getunitfor(0)->code, 1)->utick);
    
    __setu(2, 2.2, 2.2);
    imapupdateunit(map, __getunitfor(2));
    _aoi_print(map, EnumNodePrintStateNode);
    
    imapsearchfromunit(map, __getunitfor(0), result, 0.8);
    SP_EQUAL(result->tick, __getnode(__getunitfor(0)->code, 2)->utick);
    SP_TRUE(result->tick != __getnode(__getunitfor(0)->code, 2)->tick);
    SP_EQUAL(ireflistlen(result->units), 0);
    SP_TRUE(__getnode(__getunitfor(0)->code, 2)->utick != __getnode(__getunitfor(0)->code, 1)->utick);
    
    {
        
        imapsearchfromunit(map, __getunitfor(0), result, 1.1);
        SP_EQUAL(result->tick, __getnode(__getunitfor(0)->code, 1)->utick);
        
        int64_t uts = __getnode(__getunitfor(0)->code, 0)->utick;
        int64_t auts = __getnode(__getunitfor(0)->code, 1)->utick;
        int64_t aauts = __getnode(__getunitfor(0)->code, 2)->utick;
        int64_t aaauts = __getnode(__getunitfor(0)->code, 3)->utick;
        
        int64_t ts = __getnode(__getunitfor(0)->code, 0)->tick;
        int64_t ats = __getnode(__getunitfor(0)->code, 1)->tick;
        int64_t aats = __getnode(__getunitfor(0)->code, 2)->tick;
        int64_t aaats = __getnode(__getunitfor(0)->code, 3)->tick;
        
        SP_EQUAL(aaauts, aaats);
        SP_EQUAL(aauts > aats, 1);
        SP_EQUAL(auts > ats, 1);
        SP_EQUAL(auts > aauts, 1);
        SP_EQUAL(uts > ts, 1);
        
        __setu(5, 4.2, 4.2);
        imapaddunit(map, __getunitfor(5));
        _aoi_print(map, EnumNodePrintStateNode);
        
        uts = __getnode(__getunitfor(0)->code, 0)->utick;
        ts = __getnode(__getunitfor(0)->code, 0)->tick;
        
        imapsearchfromunit(map, __getunitfor(0), result, 1.1);
        SP_EQUAL(result->tick, __getnode(__getunitfor(0)->code, 1)->utick);
        SP_EQUAL(uts == ts, 1);
        
        __setu(5, 0.5, 0.5);
        imapupdateunit(map, __getunitfor(5));
        _aoi_print(map, EnumNodePrintStateNode);
        
        uts = __getnode(__getunitfor(0)->code, 0)->utick;
        ts = __getnode(__getunitfor(0)->code, 0)->tick;
        auts = __getnode(__getunitfor(0)->code, 1)->utick;
        ats = __getnode(__getunitfor(0)->code, 1)->tick;
        SP_EQUAL(uts > ts, 1);
        SP_EQUAL(auts, ats);
        SP_EQUAL(auts, uts);
        imapsearchfromunit(map, __getunitfor(0), result, 1.1);
        SP_EQUAL(result->tick, __getnode(__getunitfor(0)->code, 1)->utick);
    }
    
    isearchresultfree(result);
}

SP_CASE(searching, end) {
    for (int i=0; i<__preparecnt; ++i) {
        __unhold(i);
    }
    
    SP_TRUE(1);
    
    imapfree(map);
    
    clearalliaoicacheandmemrorystate();
}

SP_SUIT(autoreleasepool);

static int _autoreleasecount = 0;
static void _hook_iref_free(iref *ref) {
    iobjfree(ref);
    ++_autoreleasecount;
}

SP_CASE(autoreleasepool, autorelease) {

    _iautoreleasepool;

    for (int i=0; i<10; ++i) {
        iref *ref = _iautomalloc(iref);
        ref->free = _hook_iref_free;
    }

    for (int i=0; i<10; ++i) {
        inode *node = imakenode();
        node->free = _hook_iref_free;
        _iautorelease(node);
    }

    _iautoreleaseall;

    SP_EQUAL(_autoreleasecount, 20);
}
#endif
